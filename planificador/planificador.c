#include "../libs/conector.h"
#include "planificador.h"
#include "../protocolo/protocolo.h"
#include "../libs/serializador.h"
#include "../libs/deserializador.h"
#include "config.h"
#include "consola.h"

/* -- Local function prototypes -- */

static bool algorithm_is_preemptive();
static esi_information* obtain_esi_information_by_id(int esi_fd);
static void take_esi_away_from_queue(t_list* queue, int esi_fd);
static void we_must_reschedule(int* flag);
static void remove_fd(int fd, fd_set *fdset);
static void set_last_real_burst_to_zero(int esi_fd);
static int receive_coordinator_opcode(int coordinator_fd);
static char* receive_inquired_key(int coordinator_fd);
static void add_new_key_blocker(char* blocked_key);
static void send_protocol_answer(int coordinator_fd, protocol_id protocol);
static void update_blocked_esis(int* blocked_queue_flag);
static void remove_blocked_key_from_list(char* unlocked_key);
static void esi_finished(int* flag);
static double next_estimated_burst_sjf(double alpha, int last_real_burst, double last_estimated_burst);
static double next_estimated_burst_hrrn(int waited_time, int last_real_burst);
static void update_esi_information_next_estimated_burst(int esi_fd);

/* -- Global variables -- */

t_log* logger;
t_planificador_config setup;

t_list* g_locked_keys;
t_list* g_esis_sexpecting_keys;
t_list* g_esi_bursts;

t_list* g_new_queue;
t_list* g_ready_queue;
t_list* g_execution_queue;
t_list* g_blocked_queue;
t_list* g_blocked_queue_by_console;
t_list* g_finished_queue;

int main(void) {

	logger = init_log();

	setup = init_config(logger);

	char* host = setup.coordinator_ip;
	int port_coordinator = setup.coordinator_port;
	int server_port = setup.port;

	int coordinator_fd = connect_to_server(host, port_coordinator);
	log_info(logger, "Conectando al coordinador");

	if (send_handshake(coordinator_fd, SCHEDULER) != 1) {
		log_error(logger, "Fallo en en el envío del handshake");
		close(coordinator_fd);
		exit_gracefully(EXIT_FAILURE);
	}

	bool confirmation;
	int received = receive_confirmation(coordinator_fd, &confirmation);
	if (!received || !confirmation) {
		log_error(logger, "Fallo en la confirmación de recepción del handshake");
		close(coordinator_fd);
		exit_gracefully(EXIT_FAILURE);
	}

	log_info(logger, "Conectado satisfactoriamente al coordinador");

	int listener = init_listener(server_port, MAXCONN);
	log_info(logger, "Escuchando en el puerto %i...", server_port);

	fd_set connected_fds;
	fd_set read_fds;

	FD_ZERO(&connected_fds);
	FD_ZERO(&read_fds);
	FD_SET(coordinator_fd, &connected_fds);
	FD_SET(listener, &connected_fds);

	int max_fd = (listener > coordinator_fd) ? listener : coordinator_fd;
	int finished_esi_flag;
	int new_esi_flag;
	int reschedule_flag;
	int update_blocked_esi_queue_flag;
	we_must_reschedule(&reschedule_flag);

	create_administrative_structures();

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

	pthread_t tid;
	void* parasite;
	pthread_create(&tid, &attrs, init_console, parasite);

	int esi_numeric_arrival_order = 1;

	while (1) {

		read_fds = connected_fds;

		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			log_error(logger, "Error en select");
			exit(EXIT_FAILURE);
		}

		int fd;
		char* last_key_inquired;
		int old_executing_esi;

		for (fd = 0; fd <= max_fd; fd++) {

			if (FD_ISSET(fd, &read_fds) == 0) {
				continue;

			} else if (fd == listener) {

				struct sockaddr_in client_info;
				socklen_t addrlen = sizeof client_info;
				log_info(logger, "Nuevo cliente conectando...");

				int new_client_fd = accept(listener, (struct sockaddr *) &client_info, &addrlen);

				if (new_client_fd == -1) {

					log_error(logger, "Fallo al aceptar nueva conexión");
				} else {

					FD_SET(new_client_fd, &connected_fds);

					if (new_client_fd > max_fd) {

						max_fd = new_client_fd;
					}

					log_info(logger, "Socket %d conectado", new_client_fd);

					bool client_confirmation = false;
					if (receive_handshake(new_client_fd) == -1) {

						send_confirmation(new_client_fd, confirmation);
						remove_fd(new_client_fd, &connected_fds);
						log_error(logger, "Fallo en el handshake con el socket %d",
								new_client_fd);
						close(new_client_fd);
					} else {

						client_confirmation = true;
						send_confirmation(new_client_fd, confirmation);
					}

					put_new_esi_on_new_queue(new_client_fd);

					list_add(g_esi_bursts, (void*)create_esi_information(new_client_fd, esi_numeric_arrival_order));

					log_info(logger, "ESI %i conectado", esi_numeric_arrival_order);

					if (algorithm_is_preemptive()) we_must_reschedule(&reschedule_flag);

					esi_numeric_arrival_order++;
				}

			} else if (fd == coordinator_fd) {

				int opcode = receive_coordinator_opcode(fd);

				bool response;

				switch (opcode) {

				case PROTOCOL_CP_IS_THIS_KEY_BLOCKED:

					last_key_inquired = receive_inquired_key(fd);
					response = determine_if_key_is_blocked(last_key_inquired);
                    if(response) {

                    	send_protocol_answer(fd, PROTOCOL_PC_KEY_IS_BLOCKED);
                    }
                    else {

                    	send_protocol_answer(fd, PROTOCOL_PC_KEY_IS_NOT_BLOCKED);
                    }
                    break;

				case PROTOCOL_CP_IS_KEY_BLOCKED_BY_EXECUTING_ESI:

					response = key_is_blocked_by_executing_esi(last_key_inquired);

					if(response) {

						send_protocol_answer(fd, PROTOCOL_PC_KEY_BLOCKED_BY_EXECUTING_ESI);
					}
					else {

						send_protocol_answer(fd, PROTOCOL_PC_KEY_NOT_BLOCKED_BY_EXECUTING_ESI);
					}
					break;

				case PROTOCOL_CP_BLOCK_KEY:

					add_new_key_blocker(last_key_inquired);
					send_protocol_answer(fd, PROTOCOL_PC_KEY_BLOCKED_SUCCESFULLY);
					log_info(logger,"Clave %s bloqueada", last_key_inquired);
					break;

				case PROTOCOL_CP_UNLOCK_KEY:

					remove_blocked_key_from_list(last_key_inquired);
					update_blocked_esis(&update_blocked_esi_queue_flag);
					send_protocol_answer(fd, PROTOCOL_PC_KEY_UNLOCKED_SUCCESFULLY);
					log_info(logger,"Clave %s desbloqueada", last_key_inquired);
					break;
				}

			} else if (fd == *(int*) g_execution_queue->head->data) {

				int confirmation = receive_execution_result(fd);

				switch (confirmation) {

				case PROTOCOL_EP_EXECUTION_SUCCESS:
					update_waiting_time_of_ready_esis();
					update_executing_esi(fd);

					log_info(logger,"El ESI %i terminó de ejecutar una sentencia correctamente", obtain_esi_information_by_id(fd)->esi_numeric_name);

					int script_end= receive_execution_result(fd);
					if(script_end == PROTOCOL_EP_FINISHED_SCRIPT) {

						esi_finished(&finished_esi_flag);
						we_must_reschedule(&reschedule_flag);
					}
					break;

				case PROTOCOL_EP_I_AM_BLOCKED:
					list_add(g_esis_sexpecting_keys, (void*)create_esi_sexpecting_key(fd, last_key_inquired));
					move_esi_from_and_to_queue(g_execution_queue, g_blocked_queue, fd);
					update_esi_information_next_estimated_burst(fd);
					we_must_reschedule(&reschedule_flag);

					log_info(logger,"El ESI %i se encuentra bloqueado esperando la clave %s", obtain_esi_information_by_id(fd)->esi_numeric_name, last_key_inquired);
					break;

				case PROTOCOL_EP_I_BROKE_THE_LAW: /* TODO */

					log_info(logger,"El ESI %i trató de ejecutar una sentencia invalida", obtain_esi_information_by_id(fd)->esi_numeric_name);
					break;

				}

				if(finished_esi_flag == 1) {

					log_info(logger,"El ESI %i finalizó la ejecución de su script correctamente", obtain_esi_information_by_id(fd)->esi_numeric_name);
					release_resources(*(int*)g_execution_queue->head->data, &update_blocked_esi_queue_flag);
					move_esi_from_and_to_queue(g_execution_queue, g_finished_queue, *(int*)g_execution_queue->head->data);
					finished_esi_flag = 0;
				} else {

					if (update_blocked_esi_queue_flag == 1 || new_esi_flag == 1) {

						if(algorithm_is_preemptive()) {

							move_esi_from_and_to_queue(g_execution_queue, g_ready_queue, *(int*)g_execution_queue->head->data);

							we_must_reschedule(&reschedule_flag);
						}

						if (update_blocked_esi_queue_flag == 1) update_blocked_esi_queue(last_key_inquired, &update_blocked_esi_queue_flag);

						if (new_esi_flag == 1) update_new_esi_queue(&new_esi_flag);
					}

				}

				if (reschedule_flag == 1) reschedule(&reschedule_flag, &old_executing_esi);
				else authorize_esi_execution(*(int*)g_execution_queue->head->data);

			}
		}
	}

	return EXIT_SUCCESS;
}

key_blocker* create_key_blocker(char* key, int esi_id){

    key_blocker* key_blocker = malloc(sizeof(key_blocker));
    key_blocker->key = key;
    key_blocker->esi_id = esi_id;
    return key_blocker;
}

esi_sexpecting_key* create_esi_sexpecting_key(int esi_fd, char* key) {

	esi_sexpecting_key* new_esi_blocked = malloc(sizeof(esi_sexpecting_key));
	new_esi_blocked->esi_fd = esi_fd;
	new_esi_blocked->key = key;
	return new_esi_blocked;
}

esi_information* create_esi_information(int esi_id, int esi_numeric_name) {

	esi_information* esi_inf = malloc(sizeof(esi_information));
	esi_inf->esi_id = esi_id;
	esi_inf->esi_numeric_name = esi_numeric_name;
	esi_inf->last_estimated_burst = (double)setup.initial_estimation;
	esi_inf->last_real_burst = 0;
	esi_inf->waited_bursts = 0;
	return esi_inf;
}

void create_administrative_structures() {

	g_new_queue = list_create();
	g_locked_keys = list_create();
	g_esis_sexpecting_keys = list_create();
	g_esi_bursts = list_create();
	g_ready_queue = list_create();
	g_execution_queue = list_create();
	g_blocked_queue = list_create();
	g_blocked_queue_by_console = list_create();
	g_finished_queue = list_create();
}

void destroy_administrative_structures() {

	void destroy_key_blocker(void* key_blocker_) {

		free(((key_blocker*)key_blocker_)->key);
		free(key_blocker_);
	}

	list_destroy_and_destroy_elements(g_locked_keys, destroy_key_blocker);

	void destroy_esi_sexpecting_key(void* esi_sexpecting_key_) {

		free(((esi_sexpecting_key*)esi_sexpecting_key_)->key);
		free(esi_sexpecting_key_);
	}

	list_destroy_and_destroy_elements(g_esis_sexpecting_keys, destroy_esi_sexpecting_key);

	void destroy_esi_information(void* esi_inf)
	{
	    free((esi_information*)esi_inf);
	}

	list_destroy_and_destroy_elements(g_esi_bursts, destroy_esi_information);

	void delete_int_node(void* esi_fd) {

		free((int*)esi_fd);
	}

	list_destroy_and_destroy_elements(g_ready_queue, delete_int_node);
	list_destroy_and_destroy_elements(g_execution_queue, delete_int_node);
	list_destroy_and_destroy_elements(g_blocked_queue, delete_int_node);
	list_destroy_and_destroy_elements(g_blocked_queue_by_console, delete_int_node);
	list_destroy_and_destroy_elements(g_finished_queue, delete_int_node);
	list_destroy_and_destroy_elements(g_new_queue, delete_int_node);
}

void put_new_esi_on_new_queue(int new_client_fd) {

	list_add(g_new_queue,(void*)&new_client_fd);
}

void authorize_esi_execution(int esi_fd) {

	protocol_id opcode = PROTOCOL_PE_EXEC;
	if(send(esi_fd, &opcode, sizeof(opcode), 0) == -1) {

		log_error(logger, "Fallo en la autorización del ESI a ejecutar");
	}
}

void update_executing_esi(int esi_fd) {

	esi_information* executing_esi = obtain_esi_information_by_id(esi_fd);

	executing_esi->last_real_burst++;
}

int receive_execution_result(int fd) {

	protocol_id opcode;
	if (recv(fd, &opcode, sizeof(opcode), MSG_WAITALL) == -1) {

		log_error(logger, "Fallo en la confirmación de ejecución de parte del ESI");
		// TODO -- no sabes hablar, sock my port -- matar esi --no hay tiempo para vos sorete esi
	}

	return opcode;
}

void sock_my_port(int esi_fd) { /* TODO */



}

void update_waiting_time_of_ready_esis() {

	bool esi_information_in_ready(void* esi_inf) {

		bool condition(void* esi_id_in_ready) {

			return *(int*)esi_id_in_ready == ((esi_information*)esi_inf)->esi_id;
		}

		return list_any_satisfy(g_ready_queue, condition);
	}

	t_list* esis_in_ready_queue = list_filter(g_esi_bursts, esi_information_in_ready);

	void update_waited_bursts(void* esi_inf) {

		((esi_information*)esi_inf)->waited_bursts++;
	}

	list_iterate(esis_in_ready_queue, update_waited_bursts);
}

void move_esi_from_and_to_queue(t_list* from_queue, t_list* to_queue, int esi_fd) {

	take_esi_away_from_queue(from_queue, esi_fd);
	list_add(to_queue, (void*)esi_fd);
}

int schedule_esis() {

	t_scheduling_algorithm algorithm = setup.scheduling_algorithm;
	int* esi_fd;

	switch(algorithm) {

	case FIFO:
		esi_fd = (int*)g_ready_queue->head->data;
		break;

	case SJFCD:
	case SJFSD: {

		void* obtain_esi_information(void* esi_fd) {

			return (void*)obtain_esi_information_by_id(*(int*)esi_fd);
		}

		bool comparator (void* esi_inf1, void* esi_inf2) {

			int last_estimated_burst1 = ((esi_information*)esi_inf1)->last_estimated_burst;
			int last_real_burst1 = ((esi_information*)esi_inf1)->last_real_burst;
			int last_estimated_burst2 = ((esi_information*)esi_inf2)->last_estimated_burst;
			int last_real_burst2 = ((esi_information*)esi_inf2)->last_real_burst;

			return last_estimated_burst1 - last_real_burst1 <= last_estimated_burst2 - last_real_burst2;

		}

		t_list* mapped_to_sort_list = list_map(g_ready_queue, obtain_esi_information);
	 	list_sort(mapped_to_sort_list, comparator);
	 	esi_fd = &((esi_information*)mapped_to_sort_list->head->data)->esi_id;
		break;
	}

	case HRRN: {

		void* obtain_esi_information(void* esi_fd) {

			return (void*)obtain_esi_information_by_id(*(int*)esi_fd);
		}

		bool comparator (void* esi_inf1, void* esi_inf2) {

			int last_estimated_burst1 = ((esi_information*)esi_inf1)->last_estimated_burst;
			int last_estimated_burst2 = ((esi_information*)esi_inf2)->last_estimated_burst;

			return last_estimated_burst1 <= last_estimated_burst2;

		}

		t_list* mapped_to_sort_list = list_map(g_ready_queue, obtain_esi_information);
		list_sort(mapped_to_sort_list, comparator);
		esi_fd = &((esi_information*) mapped_to_sort_list->head->data)->esi_id;
		break;
	}

	}

	 esi_information* esi_inf = obtain_esi_information_by_id(*esi_fd);

	 log_info(logger,"El ESI seleccionado para ejecutar es el ESI %i", esi_inf->esi_numeric_name);

	return *esi_fd;
}

bool determine_if_key_is_blocked(char* blocked_key) {

	bool string_equals(void* key_blocker_) {

		if(strcmp(((key_blocker*)key_blocker_)->key, blocked_key) == 0) return true;
		else return false;
	}

	return list_any_satisfy(g_locked_keys, string_equals);
}

bool key_is_blocked_by_executing_esi(char* key) {

	bool condition(void* key_blocker_) {

		return strcmp(((key_blocker*)key_blocker_)->key, key) == 0 && ((key_blocker*)key_blocker_)->esi_id == *(int*)g_execution_queue->head->data;
	}

	return list_any_satisfy(g_locked_keys, condition);
}

t_list* unlock_esis(char* key_unlocked) {

	bool unlock_condition(void* esi_sexpecting) {

		return strcmp(((esi_sexpecting_key*)esi_sexpecting)->key, key_unlocked) == 0;
	}

	t_list* filtered_list = list_filter(g_esis_sexpecting_keys, unlock_condition);

	void* transformer(void* esi_sexpecting) {

		return (void*)(&((esi_sexpecting_key*)esi_sexpecting)->esi_fd);
	}

    t_list* mapped_list = list_map(filtered_list, transformer);


    bool remove_condition(void* esi_sexpecting) {

    	bool condition(void* esi_fd) {

    		return ((esi_sexpecting_key*)esi_sexpecting)->esi_fd == *(int*)esi_fd;
    	}

    	return list_any_satisfy(mapped_list, condition);
    }

    void esi_sexpecting_destroyer(void* esi_sexpecting_key_) {

    		free(((esi_sexpecting_key*)esi_sexpecting_key_)->key);
    		free(esi_sexpecting_key_);
    }

    int i, number_of_unlocked_esis;
    number_of_unlocked_esis = list_size(mapped_list);

    for(i = 0; i < number_of_unlocked_esis; i++) {

	list_remove_and_destroy_by_condition(g_esis_sexpecting_keys, remove_condition, esi_sexpecting_destroyer);

    }

	return mapped_list;
}

void update_blocked_esi_queue(char* last_key_inquired, int* update_blocked_esi_queue_flag) {

	t_list* unlocked_esis = unlock_esis(last_key_inquired);
	list_add_all(g_ready_queue, unlocked_esis);
	*update_blocked_esi_queue_flag = 0;

	log_info(logger,"Clave %s liberada", last_key_inquired);
}

void reschedule(int* reschedule_flag, int* old_executing_esi) {

	int esi_fd_to_execute = schedule_esis();
	move_esi_from_and_to_queue(g_ready_queue, g_execution_queue, esi_fd_to_execute);

	if(esi_fd_to_execute != *old_executing_esi) {

		set_last_real_burst_to_zero(esi_fd_to_execute);
	}

	authorize_esi_execution(esi_fd_to_execute);
	*old_executing_esi = esi_fd_to_execute;
	*reschedule_flag = 0;
}

void update_new_esi_queue(int* new_esi_flag) {

	list_add_all(g_ready_queue, g_new_queue);

	void int_destroyer(void* esi_fd) {

		free((int*) esi_fd);
	}

	list_clean_and_destroy_elements(g_new_queue, int_destroyer);

	*new_esi_flag = 0;
}

void release_resources(int esi_fd, int* update_blocked_esi_queue_flag) {

	log_info(logger,"Liberando recursos del ESI %i...", obtain_esi_information_by_id(esi_fd)->esi_numeric_name);

	bool condition(void* key_locker_) {

		return ((key_blocker*) key_locker_)->esi_id == esi_fd;
	}

	t_list* keys_unlocked = list_filter(g_locked_keys, condition);

	int i, keys_unlocked_quantity = list_size(keys_unlocked);

	for(i = 0; i < keys_unlocked_quantity; i++) {

	key_blocker* blocked_key = (key_blocker*)list_get(keys_unlocked, i);
	update_blocked_esi_queue(blocked_key->key, update_blocked_esi_queue_flag);
	}

	void destroy_key_blocker(void* key_blocker_) {

		free(((key_blocker*) key_blocker_)->key);
		free(key_blocker_);
	}

	bool condition2(void* esi_information_) {

		return ((esi_information*) esi_information_)->esi_id == esi_fd;
	}

	void destroy_esi_information(void* esi_inf) {

		free((esi_information*) esi_inf);
	}

	list_remove_and_destroy_by_condition(g_locked_keys, condition, destroy_key_blocker);

	list_remove_and_destroy_by_condition(g_esi_bursts, condition2, destroy_esi_information);

	log_info(logger,"Los recursos del ESI %i fueron liberados correctamente", obtain_esi_information_by_id(esi_fd)->esi_numeric_name);

}

void exit_gracefully(int status) {

	log_info(logger, "La ejecución del planificador terminó");

	log_destroy(logger);

	destroy_administrative_structures();

	exit(status);
}

/* ---------------------------------- PRIVATE FUNCTIONS ---------------------------------- */

static void remove_fd(int fd, fd_set *fdset) {
	FD_CLR(fd, fdset);
	log_info(logger, "El socket %d fue echado", fd);
	close(fd);
}

static bool algorithm_is_preemptive() {

	int algorithm_type = setup.scheduling_algorithm;
	switch (algorithm_type) {

	case 0:
		return true;
		break;
	default:
		return false;
		break;
	}
}

static esi_information* obtain_esi_information_by_id(int esi_fd){

	bool equal_condition(void* esi_inf) {

		return ((esi_information*)esi_inf)->esi_id == esi_fd;
	}

	return list_find(g_esi_bursts, equal_condition);
 }

static void take_esi_away_from_queue(t_list* queue, int esi_fd) {

	bool remove_condition(void* esi_to_delete) {

		return *(int*)esi_to_delete == esi_fd;
	}

	void destroy_esi_fd(void* esi_fd_) {

		free((int*)esi_fd_);
	}

	list_remove_and_destroy_by_condition(queue, remove_condition, destroy_esi_fd);
}

static void we_must_reschedule(int* flag) {

	*flag = 1;
}

static void esi_finished(int* flag) {

	*flag = 1;
}

static void set_last_real_burst_to_zero(int esi_fd) {

	esi_information* esi_inf = obtain_esi_information_by_id(esi_fd);
	esi_inf->last_real_burst = 0;
}

static int receive_coordinator_opcode(int coordinator_fd) {

	int opcode;
	if(recv(coordinator_fd, &opcode, sizeof(int), MSG_WAITALL) == -1) {

		log_error(logger, "Fallo en la comunicación con el coordinador al recibir el código de operación");
	    exit_gracefully(EXIT_FAILURE); //TODO -- Si el coordinador murió horrendamente, qué hacemos?
	}
	return opcode;
}

static char* receive_inquired_key(int coordinator_fd) {

	char* key;
	int result = recv_package_variable(coordinator_fd, (void**)&key);

	if(result == -2 || result == -3) {

		log_error(logger, "Error al recibir la clave de parte del coordinador");
		exit_gracefully(EXIT_FAILURE); //TODO -- Si el coordinador murió horrendamente, qué hacemos?
	}

	return key;
}

static void add_new_key_blocker(char* blocked_key) {

	key_blocker* key_blocker_ = malloc(sizeof(key_blocker));
	memcpy(key_blocker_->key, blocked_key, strlen(blocked_key));
	memcpy(&(key_blocker_->esi_id), (int*)g_execution_queue->head->data, sizeof(int));

	free(blocked_key);
	list_add(g_locked_keys, (void*)key_blocker_);
}

static void send_protocol_answer(int coordinator_fd, protocol_id protocol) {

	if (send(coordinator_fd, &protocol, sizeof(protocol), 0) == -1) {

		log_error(logger, "Fallo en el envío del status de la clave al coordinador");
	}
}

static void update_blocked_esis(int* blocked_queue_flag) {

	*blocked_queue_flag = 1;
}

static void remove_blocked_key_from_list(char* unlocked_key) {

	bool remove_condition(void* key_blocker_) {

		return strcmp(((key_blocker*)key_blocker_)->key, unlocked_key) == 0;
	}

	void key_blocker_destroyer(void* key_blocker_) {

		free(((key_blocker*) key_blocker_)->key);
		free(key_blocker_);
	}

	list_remove_and_destroy_by_condition(g_locked_keys, remove_condition, key_blocker_destroyer);
}

static double next_estimated_burst_sjf(double alpha, int last_real_burst, double last_estimated_burst) {

	return alpha*last_real_burst + (1 - alpha)*last_estimated_burst;

}

static double next_estimated_burst_hrrn(int waited_time, int last_real_burst) {

	return waited_time/last_real_burst;
}

static void update_esi_information_next_estimated_burst(int esi_fd) {

	esi_information* esi_inf = obtain_esi_information_by_id(esi_fd);

	if(setup.scheduling_algorithm == SJFCD || setup.scheduling_algorithm == SJFSD) {

		esi_inf->last_estimated_burst = next_estimated_burst_sjf(setup.alpha, esi_inf->last_real_burst, esi_inf->last_estimated_burst);
		esi_inf->last_real_burst = 0;
		esi_inf->waited_bursts = 0;
	}

	if(setup.scheduling_algorithm == HRRN) {

		esi_inf->last_estimated_burst = next_estimated_burst_hrrn(esi_inf->waited_bursts, esi_inf->last_real_burst);
		esi_inf->last_real_burst = 0;
		esi_inf->waited_bursts = 0;
	}
}

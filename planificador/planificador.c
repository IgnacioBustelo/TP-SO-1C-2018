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
static void set_waiting_time_to_zero(int esi_fd);
static int receive_coordinator_opcode(int coordinator_fd);
static char* receive_inquired_key(int coordinator_fd);
static void add_new_key_blocker(char* blocked_key);
static void send_protocol_answer(int coordinator_fd, protocol_id protocol);
static void update_blocked_esis(int* blocked_queue_flag);
static void new_esi_detected(int* new_esi_flag);
static void remove_blocked_key_from_list(char* unlocked_key);
static void esi_finished(int* flag);
static double next_estimated_burst_sjf(double alpha, int last_real_burst, double last_estimated_burst);
static double next_estimated_burst_hrrn(int waited_time, int last_real_burst);
static void update_esi_information_next_estimated_burst(int esi_fd);
static void block_by_console_procedure();

/* -- Global variables -- */

t_log* logger;
t_planificador_config setup;

t_list* g_locked_keys;
t_list* g_esis_sexpecting_keys;
t_list* g_esi_bursts;
t_list* g_new_blocked_by_console_esis;

t_list* g_new_queue;
t_list* g_ready_queue;
t_list* g_execution_queue;
t_list* g_blocked_queue;
t_list* g_blocked_queue_by_console;
t_list* g_finished_queue;

int finished_esi_flag = 0;
int new_esi_flag = 0;
int reschedule_flag = 0;
int update_blocked_esi_queue_flag = 0;

int scheduler_paused_flag = 0;
int block_esi_by_console_flag = 0;
int unlock_esi_by_console_flag = 0;
char* last_unlocked_key_by_console;

fd_set connected_fds;
int max_fd;
int g_coordinator_fd;

int main(void) {

	logger = init_log();

	setup = init_config(logger);

	char* host = setup.coordinator_ip;
	int port_coordinator = setup.coordinator_port;
	int server_port = setup.port;

	g_coordinator_fd = connect_to_server(host, port_coordinator);
	log_info(logger, "Conectando al coordinador");

	if (send_handshake(g_coordinator_fd, SCHEDULER) != 1) {
		log_error(logger, "Fallo en en el envío del handshake");
		close(g_coordinator_fd);
		exit_gracefully(EXIT_FAILURE);
	}

	bool confirmation;
	int received = receive_confirmation(g_coordinator_fd, &confirmation);
	if (!received || !confirmation) {
		log_error(logger, "Fallo en la confirmación de recepción del handshake");
		close(g_coordinator_fd);
		exit_gracefully(EXIT_FAILURE);
	}

	log_info(logger, "Conectado satisfactoriamente al coordinador");

	int listener = init_listener(server_port, MAXCONN);
	log_info(logger, "Escuchando en el puerto %i...", server_port);

	fd_set read_fds;

	FD_ZERO(&connected_fds);
	FD_ZERO(&read_fds);
	FD_SET(g_coordinator_fd, &connected_fds);
	FD_SET(listener, &connected_fds);
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	max_fd = (listener > g_coordinator_fd) ? listener : g_coordinator_fd;

	we_must_reschedule(&reschedule_flag);

	create_administrative_structures();

	pthread_attr_t attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

	pthread_t tid;
	void* parasite;
	pthread_create(&tid, &attrs, init_console, parasite);

	int esi_numeric_arrival_order = 1;
	int executing_esi = -1;

	while (1) {

		read_fds = connected_fds;

		if (select(max_fd + 1, &read_fds, NULL, NULL, &tv) == -1) {
			log_error(logger, "Error en select");
			exit(EXIT_FAILURE);
		}

		int fd;
		char* last_key_inquired; //Hay que ver dónde hacer el free cuando termina de usarla -- TODO

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
					} else {

						client_confirmation = true;
						send_confirmation(new_client_fd, confirmation);
					}

					put_new_esi_on_new_queue(new_client_fd);

					list_add(g_esi_bursts, (void*)create_esi_information(new_client_fd, esi_numeric_arrival_order));

					log_info(logger, "ESI %i conectado", esi_numeric_arrival_order);

					new_esi_detected(&new_esi_flag);

					if (algorithm_is_preemptive()) we_must_reschedule(&reschedule_flag);

					esi_numeric_arrival_order++;
				}

			} else if (fd == g_coordinator_fd) {

				int opcode = receive_coordinator_opcode(fd);

				bool response;

				switch (opcode) {

				case PROTOCOL_CP_IS_THIS_KEY_BLOCKED:

					last_key_inquired = receive_inquired_key(fd);

					log_info(logger, "El coordinador preguntó si la clave %s está bloqueada", last_key_inquired);

					response = determine_if_key_is_blocked(last_key_inquired);
                    if(response) {

                    	send_protocol_answer(fd, PROTOCOL_PC_KEY_IS_BLOCKED);
                    	log_info(logger, "La clave solicitada estaba bloqueada");
                    }
                    else {

                    	send_protocol_answer(fd, PROTOCOL_PC_KEY_IS_NOT_BLOCKED);
                    	log_info(logger, "La clave solicitada no estaba bloqueada");
                    }
                    break;

				case PROTOCOL_CP_IS_KEY_BLOCKED_BY_EXECUTING_ESI:

					response = key_is_blocked_by_executing_esi(last_key_inquired);

					if(response) {

						send_protocol_answer(fd, PROTOCOL_PC_KEY_BLOCKED_BY_EXECUTING_ESI);
						log_info(logger, "La clave %s que fue solicitada fue bloqueada por el ESI en ejecución");
					}
					else {

						send_protocol_answer(fd, PROTOCOL_PC_KEY_NOT_BLOCKED_BY_EXECUTING_ESI);
						log_info(logger, "La clave %s que fue solicitada no fue bloqueada por el ESI en ejecución");
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

			} else if (fd == executing_esi) {

				int confirmation = receive_execution_result(fd);

				switch (confirmation) {

				case PROTOCOL_EP_EXECUTION_SUCCESS:
					update_waiting_time_of_ready_esis();
					update_executing_esi(fd);

					log_info(logger,"El ESI %i terminó de ejecutar una sentencia correctamente", obtain_esi_information_by_id(fd)->esi_numeric_name);

					int script_end= receive_execution_result(fd);
					if(script_end == PROTOCOL_EP_FINISHED_SCRIPT) {

						esi_finished(&finished_esi_flag);

						if(!list_is_empty(g_ready_queue)) {

						we_must_reschedule(&reschedule_flag);
						}
					}
					break;

				case PROTOCOL_EP_I_AM_BLOCKED:
					list_add(g_esis_sexpecting_keys, (void*)create_esi_sexpecting_key(fd, last_key_inquired));
					move_esi_from_and_to_queue(g_execution_queue, g_blocked_queue, fd);
					update_esi_information_next_estimated_burst(fd);

					if (!list_is_empty(g_ready_queue)) {

						we_must_reschedule(&reschedule_flag);
					}

					log_info(logger,"El ESI %i se encuentra bloqueado esperando la clave %s", obtain_esi_information_by_id(fd)->esi_numeric_name, last_key_inquired);
					break;

				case PROTOCOL_EP_I_BROKE_THE_LAW:

					log_info(logger,"El ESI %i trató de ejecutar una sentencia invalida", obtain_esi_information_by_id(fd)->esi_numeric_name);
					sock_my_port(fd);

					esi_finished(&finished_esi_flag);

					if (!list_is_empty(g_ready_queue)) {

						we_must_reschedule(&reschedule_flag);
					}
					break;

				}

				if(finished_esi_flag == 1) {

					log_info(logger,"El ESI %i finalizó la ejecución de su script correctamente", obtain_esi_information_by_id(fd)->esi_numeric_name);
					release_resources(*(int*)g_execution_queue->head->data, &update_blocked_esi_queue_flag);
					move_esi_from_and_to_queue(g_execution_queue, g_finished_queue, *(int*)g_execution_queue->head->data);
					executing_esi = -1;
					finished_esi_flag = 0;
				} else {

					if (update_blocked_esi_queue_flag == 1 || new_esi_flag == 1) {

						if(algorithm_is_preemptive()) {

							move_esi_from_and_to_queue(g_execution_queue, g_ready_queue, *(int*)g_execution_queue->head->data);

							we_must_reschedule(&reschedule_flag);
						}

						if (update_blocked_esi_queue_flag == 1) update_blocked_esi_queue(last_key_inquired, &update_blocked_esi_queue_flag);

						if (unlock_esi_by_console_flag == 1) update_blocked_by_console_esi_queue();

						if (new_esi_flag == 1) update_new_esi_queue(&new_esi_flag);

						if(block_esi_by_console_flag == 1) block_by_console_procedure();

					}

				}

				if (reschedule_flag == 1){

					reschedule(&reschedule_flag, &executing_esi);
				}
				else if (!list_is_empty(g_execution_queue)) {

					authorize_esi_execution(*(int*)g_execution_queue->head->data);
				}

			} else sock_my_port(fd);
		}

		if(list_is_empty(g_execution_queue) && !list_is_empty(g_new_queue) && scheduler_paused_flag != 1) {

			if (update_blocked_esi_queue_flag == 1) update_blocked_esi_queue(last_key_inquired, &update_blocked_esi_queue_flag);

			if (unlock_esi_by_console_flag == 1) update_blocked_by_console_esi_queue();

			if (new_esi_flag == 1) update_new_esi_queue(&new_esi_flag);

			if (block_esi_by_console_flag == 1) block_by_console_procedure();

			reschedule(&reschedule_flag, &executing_esi);
		}

	}

	return EXIT_SUCCESS;
}

key_blocker* create_key_blocker(char* key, int esi_id){

    key_blocker* key_blocker_ = malloc(sizeof(key_blocker));
    key_blocker_->key = strdup(key);
    key_blocker_->esi_id = esi_id;
    return key_blocker_;
}

esi_sexpecting_key* create_esi_sexpecting_key(int esi_fd, char* key) {

	esi_sexpecting_key* new_esi_blocked = malloc(sizeof(esi_sexpecting_key));
	new_esi_blocked->esi_fd = esi_fd;
	new_esi_blocked->key = strdup(key);
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
	g_new_blocked_by_console_esis = list_create();

	int i = 0;
	while(setup.blocked_keys[i] != NULL) {

		list_add(g_locked_keys, (void*)create_key_blocker(setup.blocked_keys[i], -1));
		i++;
	}
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

	list_destroy_and_destroy_elements(g_new_blocked_by_console_esis, destroy_esi_sexpecting_key);
}

void put_new_esi_on_new_queue(int new_client_fd) {

	int* client_fd = malloc(sizeof(int));
	*client_fd = new_client_fd;

	list_add(g_new_queue,(void*)client_fd);
}

void authorize_esi_execution(int esi_fd) {

	protocol_id opcode = PROTOCOL_PE_EXEC;
	if(send(esi_fd, &opcode, sizeof(opcode), 0) == -1) {

		log_error(logger, "Fallo en la autorización del ESI a ejecutar");
		sock_my_port(esi_fd);
	}
}

void update_executing_esi(int esi_fd) {

	esi_information* executing_esi = obtain_esi_information_by_id(esi_fd);

	executing_esi->last_real_burst++;
}

int receive_execution_result(int fd) {

	protocol_id opcode;
	if (recv(fd, &opcode, sizeof(opcode), MSG_WAITALL) != sizeof(opcode)) {

		log_error(logger, "Fallo en la confirmación de ejecución de parte del ESI");
		sock_my_port(fd);
	}

	return opcode;
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
	int* esi = malloc(sizeof(int));
	*esi = esi_fd;
	list_add(to_queue, (void*)esi);
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

t_list* unlock_esis(char* unlocked_key) {

	bool unlock_condition(void* esi_sexpecting) {

		return strcmp(((esi_sexpecting_key*)esi_sexpecting)->key, unlocked_key) == 0;
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

    list_remove_and_destroy_by_condition(g_esis_sexpecting_keys, remove_condition, esi_sexpecting_destroyer);

	return mapped_list;
}

void update_blocked_esi_queue(char* last_key_inquired, int* update_blocked_esi_queue_flag) {

	t_list* unlocked_esis = unlock_esis(last_key_inquired);

	if(unlocked_esis->elements_count == 0) {

		log_info(logger, "Ningun esi estaba bloqueado por la clave %s", last_key_inquired);
	} else {

	list_add_all(g_ready_queue, unlocked_esis);

	void show_unlocked_esi(void* esi_fd) {

		log_info(logger, "El ESI %i se ha desbloqueado", obtain_esi_information_by_id(*(int*)esi_fd)->esi_numeric_name);
	}

	list_iterate(unlocked_esis, show_unlocked_esi);
	}

	*update_blocked_esi_queue_flag = 0;

	log_info(logger,"Clave %s liberada", last_key_inquired);
}

void update_blocked_by_console_esi_queue() {

	bool find_first_blocked_esi(void* esi_sexpecting_key_) {

		return strcmp(((esi_sexpecting_key*)esi_sexpecting_key_)->key, last_unlocked_key_by_console) == 0;
	}

	esi_sexpecting_key* esi_sexpecting_key_ = list_find(g_esis_sexpecting_keys, find_first_blocked_esi);

	if(esi_sexpecting_key_ != NULL) {

		move_esi_from_and_to_queue(g_blocked_queue_by_console, g_ready_queue, esi_sexpecting_key_->esi_fd);

		log_info(logger, "El ESI %i fue desbloqueado por consola al desbloquear la clave %s", obtain_esi_information_by_id(esi_sexpecting_key_->esi_fd)->esi_numeric_name, last_unlocked_key_by_console);

		bool remove_condition(void* esi_sexpecting_key2) {

			bool condition1 = ((esi_sexpecting_key*)esi_sexpecting_key2)->esi_fd == esi_sexpecting_key_->esi_fd;
			bool condition2 = strcmp(((esi_sexpecting_key*)esi_sexpecting_key2)->key, esi_sexpecting_key_->key) == 0;
			return condition1 && condition2;
		}

		void destroy_esi_sexpecting_key(void* esi_sexpecting_key_) {

			free(((esi_sexpecting_key*) esi_sexpecting_key_)->key);
			free(esi_sexpecting_key_);
		}

		list_remove_and_destroy_by_condition(g_esis_sexpecting_keys, remove_condition, destroy_esi_sexpecting_key);
	}
	else log_info(logger, "Ningún ESI está bloqueado por consola esperando la clave %s", last_unlocked_key_by_console);
}

void reschedule(int* reschedule_flag, int* old_executing_esi) {

	int esi_fd_to_execute = schedule_esis();
	move_esi_from_and_to_queue(g_ready_queue, g_execution_queue, esi_fd_to_execute);

	if(esi_fd_to_execute != *old_executing_esi) {

		set_last_real_burst_to_zero(esi_fd_to_execute);
	}

	set_waiting_time_to_zero(esi_fd_to_execute);

	authorize_esi_execution(esi_fd_to_execute);
	*old_executing_esi = esi_fd_to_execute;
	*reschedule_flag = 0;
}

void update_new_esi_queue(int* new_esi_flag) {

	list_add_all(g_ready_queue, g_new_queue);

	list_clean(g_new_queue);

	*new_esi_flag = 0;
}

void release_resources(int esi_fd, int* update_blocked_esi_queue_flag) {

	int esi_numeric_order = obtain_esi_information_by_id(esi_fd)->esi_numeric_name;

	log_info(logger,"Liberando recursos del ESI %i...", esi_numeric_order);

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

	log_info(logger,"Los recursos del ESI %i fueron liberados correctamente", esi_numeric_order);

}

void sock_my_port(int esi_fd) {

	log_info(logger, "El ESI %i murió horrendamente", obtain_esi_information_by_id(esi_fd)->esi_numeric_name);

	bool find_dead_esi(void* esi_fd_) {

		return *(int*) esi_fd_ == esi_fd;
	}

	void bury_esi(t_list* list) {

		release_resources(esi_fd, &update_blocked_esi_queue_flag);
		move_esi_from_and_to_queue(list, g_finished_queue, esi_fd);
	}

	int* dead_esi;

	dead_esi = list_find(g_new_queue, find_dead_esi);
	if (dead_esi != NULL) bury_esi(g_new_queue);

	dead_esi = list_find(g_ready_queue, find_dead_esi);
	if (dead_esi != NULL) bury_esi(g_ready_queue);

	dead_esi = list_find(g_execution_queue, find_dead_esi);
	if (dead_esi != NULL) bury_esi(g_execution_queue);

	dead_esi = list_find(g_blocked_queue, find_dead_esi);
	if (dead_esi != NULL) bury_esi(g_blocked_queue);

	dead_esi = list_find(g_blocked_queue_by_console, find_dead_esi);
	if (dead_esi != NULL) bury_esi(g_blocked_queue_by_console);

	remove_fd(esi_fd, &connected_fds);
}

void kaboom_baby() {

	int fd;
	for(fd = 0; max_fd + 1; fd++) {

		if(FD_ISSET(fd, &connected_fds) == 0 && fd != g_coordinator_fd) sock_my_port(fd);
		else if(FD_ISSET(fd, &connected_fds) == 0 && fd == g_coordinator_fd) remove_fd(fd, &connected_fds);
	}

	exit_gracefully(EXIT_FAILURE);
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

	case 1:
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

static void set_waiting_time_to_zero(int esi_fd) {

	esi_information* esi_inf = obtain_esi_information_by_id(esi_fd);
	esi_inf->waited_bursts = 0;
}

static int receive_coordinator_opcode(int coordinator_fd) {

	int opcode;
	if(recv(coordinator_fd, &opcode, sizeof(int), MSG_WAITALL) != sizeof(int)) {

		log_error(logger, "Fallo en la comunicación con el coordinador al recibir el código de operación");
		kaboom_baby(coordinator_fd);
	}
	return opcode;
}

static char* receive_inquired_key(int coordinator_fd) {

	char* key;
	int result = recv_package_variable(coordinator_fd, (void**)&key);

	if(result == -2 || result == -3) {

		log_error(logger, "Error al recibir la clave de parte del coordinador");
		kaboom_baby(coordinator_fd);
	}

	return key;
}

static void add_new_key_blocker(char* blocked_key) {

	list_add(g_locked_keys, (void*) create_key_blocker(blocked_key,*(int*)g_execution_queue->head->data));
}

static void send_protocol_answer(int coordinator_fd, protocol_id protocol) {

	if (send(coordinator_fd, &protocol, sizeof(protocol), 0) == -1) {

		log_error(logger, "Fallo en el envío del status de la clave al coordinador");
	}
}

static void update_blocked_esis(int* blocked_queue_flag) {

	*blocked_queue_flag = 1;
}

static void new_esi_detected(int* new_esi_flag) {

	*new_esi_flag = 1;
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

static void block_by_console_procedure() {

	void* trans(void* esi_sexpecting_key_) {

		bool condition(void* esi_inf) {

			return ((esi_information*)esi_inf)->esi_numeric_name == ((esi_sexpecting_key*)esi_sexpecting_key_)->esi_fd;
		}

	    esi_information* esi_inf = list_find(g_esi_bursts, condition);

	    ((esi_sexpecting_key*)esi_sexpecting_key_)->esi_fd = esi_inf->esi_id;

		return esi_sexpecting_key_;
	}

	t_list* mapped_list = list_map(g_new_blocked_by_console_esis, trans);

	void* transformer(void* esi_key) {

		return (void*)(((esi_sexpecting_key*)esi_key)->esi_fd);
	}

	t_list* esis = list_map(mapped_list, transformer);

	bool is_executing(void* esi_fd) {

		return *(int*)g_execution_queue->head->data == *(int*)esi_fd;
	}

	if(!list_is_empty(g_execution_queue) && list_any_satisfy(esis, is_executing)) {

		int* esi_executing = list_find(esis, is_executing);

		bool condition(void* esi_sexpecting) {

			return ((esi_sexpecting_key*)esi_sexpecting)->esi_fd == *esi_executing;
		}

		esi_sexpecting_key* esi_sexpecting_key_exec = list_find(mapped_list, condition);
		list_add(g_esis_sexpecting_keys, (void*)create_esi_sexpecting_key(esi_sexpecting_key_exec->esi_fd, esi_sexpecting_key_exec->key));

		move_esi_from_and_to_queue(g_execution_queue, g_new_blocked_by_console_esis, *esi_executing);

		log_info(logger, "El ESI %i se ha bloqueado por consola y ahora está esperando la liberación de la clave %s", obtain_esi_information_by_id(*esi_executing)->esi_numeric_name, esi_sexpecting_key_exec->key);
	}

	bool is_in_ready(void* esi_fd) {

		bool condition4(void* esi_fd_in_ready) {

			return *(int*)esi_fd_in_ready == *(int*)esi_fd;
		}

		return list_any_satisfy(g_ready_queue, condition4);
	}

	if (!list_is_empty(g_ready_queue) && list_any_satisfy(esis, is_in_ready)) {

		t_list* filtered_list = list_filter(esis, is_in_ready);

		void closure(void* esi_fd) {

			bool condition(void* esi_sexpecting) {

				return ((esi_sexpecting_key*) esi_sexpecting)->esi_fd == *(int*)esi_fd;
			}

			esi_sexpecting_key* esi_sexpecting_key_ready = list_find(mapped_list, condition);
			list_add(g_esis_sexpecting_keys, (void*)create_esi_sexpecting_key(esi_sexpecting_key_ready->esi_fd, esi_sexpecting_key_ready->key));

			move_esi_from_and_to_queue(g_ready_queue, g_blocked_queue_by_console, *(int*)esi_fd);

			log_info(logger, "El ESI %i se ha bloqueado por consola y ahora está esperando la liberación de la clave %s", obtain_esi_information_by_id(esi_sexpecting_key_ready->esi_fd)->esi_numeric_name, esi_sexpecting_key_ready->key);
		}

		list_iterate(filtered_list, closure);
	}

	bool not_in_ready_nor_exec(void* esi_fd) {

		return !is_executing(*(int*)esi_fd) && !is_in_ready(*(int*)esi_fd);
	}

	t_list* esis_that_cant_be_blocked_by_console = list_filter(esis, not_in_ready_nor_exec);
	if(esis_that_cant_be_blocked_by_console->elements_count != 0) {

		void show_esi_that_wasnt_blocked(void* esi_fd) {

			log_error(logger, "El ESI %i no pudo bloquearse porque no se encuentra en ready ni tampoco en ejecución", obtain_esi_information_by_id(*(int*)esi_fd)->esi_numeric_name);
		}

		list_iterate(esis_that_cant_be_blocked_by_console, show_esi_that_wasnt_blocked);
	}

	void destroy_esi_sexpecting_key(void* esi_sexpecting_key_) {

		free(((esi_sexpecting_key*) esi_sexpecting_key_)->key);
		free(esi_sexpecting_key_);
	}

	list_clean_and_destroy_elements(g_new_blocked_by_console_esis, destroy_esi_sexpecting_key);

	block_esi_by_console_flag = 0;
}

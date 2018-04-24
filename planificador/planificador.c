#include "../libs/conector.h"
#include "planificador.h"
#include "../protocolo/protocolo.h"
#include "../libs/serializador.h"
#include "config.h"

/* -- Local function prototypes -- */

static bool algorithm_is_preemptive();
static esi_information* obtain_esi_information_by_id(int esi_fd);
static void take_esi_away_from_queue(t_list* queue, int esi_fd);
static void we_must_reschedule(int* flag);
static void remove_fd(int fd, fd_set *fdset);
static void set_last_real_burst_to_zero(int esi_fd);
static int receive_coordinator_opcode(int coordinator_fd);
static char* receive_blocked_key(int coordinator_fd);
static void add_new_blocked_key(char* blocked_key);
static void send_key_status(int coordinator_fd, protocol_id protocol);

/* -- Global variables -- */

t_log* logger;
t_planificador_config setup;

t_list* g_locked_keys;
t_list* g_esi_bursts;

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
	log_info(logger, "Connecting to the coordinator...");

	if (send_handshake(coordinator_fd, SCHEDULER) != 1) {
		log_error(logger, "Failure in send_handshake");
		close(coordinator_fd);
		exit_gracefully(EXIT_FAILURE);
	}

	bool confirmation;
	int received = receive_confirmation(coordinator_fd, &confirmation);
	if (!received || !confirmation) {
		log_error(logger, "Failure in confirmation reception");
		close(coordinator_fd);
		exit_gracefully(EXIT_FAILURE);
	}

	log_info(logger, "Succesfully connected to the coordinator");

	int listener = init_listener(server_port, MAXCONN);
	log_info(logger, "Listening on port %i...", server_port);

	fd_set connected_fds;
	fd_set read_fds;

	FD_ZERO(&connected_fds);
	FD_ZERO(&read_fds);
	FD_SET(coordinator_fd, &connected_fds);
	FD_SET(listener, &connected_fds);

	int max_fd = (listener > coordinator_fd) ? listener : coordinator_fd;
	int flag;
	we_must_reschedule(&flag);

	create_administrative_structures();

	while (1) {
		read_fds = connected_fds;

		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
			log_error(logger, "Select error");
			exit(EXIT_FAILURE);
		}

		int fd;

		for (fd = 0; fd <= max_fd; fd++) {

			if (FD_ISSET(fd, &read_fds) == 0) {
				continue;

			} else if (fd == listener) {

				struct sockaddr_in client_info;
				socklen_t addrlen = sizeof client_info;
				log_info(logger, "New client connecting...");

				int new_client_fd = accept(listener,
						(struct sockaddr *) &client_info, &addrlen);

				if (new_client_fd == -1) {

					log_error(logger, "Accept error");
				} else {

					FD_SET(new_client_fd, &connected_fds);

					if (new_client_fd > max_fd) {

						max_fd = new_client_fd;
					}

					log_info(logger, "Socket %d connected", new_client_fd);

					bool client_confirmation = false;
					if (receive_handshake(new_client_fd) == -1) {

						send_confirmation(new_client_fd, confirmation);
						remove_fd(new_client_fd, &connected_fds);
						log_error(logger, "Handshake fail with socket %d",
								new_client_fd);
						close(new_client_fd);
					} else {

						client_confirmation = true;
						send_confirmation(new_client_fd, confirmation);
					}

					list_add(g_esi_bursts,
							(void*) create_esi_information(new_client_fd));
					put_new_esi_on_ready_queue(new_client_fd);

					if (algorithm_is_preemptive())
						we_must_reschedule(&flag);
				}

			} else if (fd == coordinator_fd) {

				/* TODO -- El coordinador se comunica con el planificador*/

				int opcode = receive_coordinator_opcode(fd);

				switch (opcode) {

				case PROTOCOL_CP_IS_THIS_KEY_BLOCKED: { // si la clave esta bloqueada por el esi en ejecucion
					char* blocked_key = receive_blocked_key(fd);
					bool response = determine_if_key_is_blocked(blocked_key);

					if (response) {

						//caso en el que el esi en ejecucion quiere usar una clave bloqueada por otro esi -- se podría hacer después de la respuesta del ESI
						move_esi_from_and_to_queue(g_execution_queue,
								g_blocked_queue,
								*(int*) g_execution_queue->head->data);
						we_must_reschedule(&flag);
						send_key_status(fd, PROTOCOL_PC_ESI_BLOCKED);

					} else {

						//caso en el que un esi quiere usar una clave no bloqueada
						add_new_blocked_key(blocked_key);
						send_key_status(fd, PROTOCOL_PC_ESI_CAN_TAKE_KEY);
					}
					//otro caso: un esi quiere usar una clave bloqueada por sí mismo

				}
				}

				//Si hicieron un GET el coordinador le debería decir qué clave fue bloqueada y debería agregar al esi en ejecución junto con esta clave a la lista key_blocker

				//si se pregunta por clave bloqueada, se contesta y nunca se replanifica
				//si se avisa el desbloqueo, se desbloquea y si es con desalojo flag de replanificar en 1
			} else if (fd == *(int*) g_execution_queue->head->data) {

				int confirmation = receive_execution_result(fd);

				switch (confirmation) {

				case PROTOCOL_EP_EXECUTION_SUCCESS:
					update_waiting_time_of_ready_esis();
					update_executing_esi(fd);
					break;

				default:
					break;
				}
			}
		}

		/* Hay que replanificar */
		if (flag == 1) {

			int esi_fd_to_execute = schedule_esis();
			move_esi_from_and_to_queue(g_ready_queue, g_execution_queue,
					esi_fd_to_execute);
			set_last_real_burst_to_zero(esi_fd_to_execute);
			flag = 0;
			authorize_esi_execution(esi_fd_to_execute);
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

esi_information* create_esi_information(int esi_id) {

	esi_information* esi_inf = malloc(sizeof(esi_information));
	esi_inf->esi_id = esi_id;
	esi_inf->next_left_estimated_burst = (double)setup.initial_estimation;
	esi_inf->last_estimated_burst = esi_inf->next_left_estimated_burst;
	esi_inf->last_real_burst = 0;
	esi_inf->waited_bursts = 0;
	return esi_inf;
}

void create_administrative_structures() {

	g_locked_keys = list_create();
	g_esi_bursts = list_create();
	g_ready_queue = list_create();
	g_execution_queue = list_create();
	g_blocked_queue = list_create();
	g_blocked_queue_by_console = list_create();
	g_finished_queue = list_create();
}

void destroy_administrative_structures() {

	void destroy_key_blocker(void* key_blocker_)
	{
		free(((key_blocker*)key_blocker_)->key);
		free(key_blocker_);
	}

	list_destroy_and_destroy_elements(g_locked_keys, destroy_key_blocker);

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
}

void put_new_esi_on_ready_queue(int new_client_fd) {

	list_add(g_ready_queue,(void*)&new_client_fd);
}

void authorize_esi_execution(int esi_fd) {

	protocol_id opcode = PROTOCOL_PE_EXEC;
	if(send(esi_fd, &opcode, sizeof(opcode), 0) == -1) {

		log_error(logger, "Failure in authorization of esi execution");
	}
}

int receive_confirmation_from_esi(int fd) {

	int* buffer;
	if(recv(fd, buffer, sizeof(int), MSG_WAITALL) == -1) {

		log_error(logger, "Confirmation failed");
		//TODO -- no sabes hablar, sock my port -- matar esi --no hay tiempo para vos sorete esi
	}
	return *buffer;
}

void update_executing_esi(int esi_fd) {

	esi_information* executing_esi = obtain_esi_information_by_id(esi_fd);

	executing_esi->last_real_burst++;
	executing_esi->next_left_estimated_burst--;
}

int receive_execution_result(int fd) {

	protocol_id opcode;
	if (recv(fd, &opcode, sizeof(opcode), MSG_WAITALL) == -1) {

		log_error(logger, "Error in ESI confirmation execution");
		// TODO -- no sabes hablar, sock my port -- matar esi --no hay tiempo para vos sorete esi
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
	list_add(to_queue, (void*)esi_fd);
}

int schedule_esis() { //TODO

	t_scheduling_algorithm algorithm = setup.scheduling_algorithm;
	int* esi_fd;

	switch(algorithm) {

	case FIFO:
		esi_fd = (int*)g_ready_queue->head->data;
		break;

	case SJFCD:
		break;

	case SJFSD:
		break;

	case HRRN:
		break;
	}

	return *esi_fd;
}

bool determine_if_key_is_blocked(char* blocked_key) {

	bool string_equals(void* key_blocker_) {

		if(strcmp(((key_blocker*)key_blocker_)->key, blocked_key) == 0) return true;
		else return false;
	}

	return list_any_satisfy(g_locked_keys, string_equals);
}


void exit_gracefully(int status) {

	log_info(logger, "Scheduler execution ended");

	log_destroy(logger);

	destroy_administrative_structures();

	exit(status);
}

/* --- PRIVATE FUNCTIONS --- */

static void remove_fd(int fd, fd_set *fdset) {
	FD_CLR(fd, fdset);
	log_info(logger, "Socket %d kicked out", fd);
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

static void set_last_real_burst_to_zero(int esi_fd) {

	esi_information* esi_inf = obtain_esi_information_by_id(esi_fd);
	esi_inf->last_real_burst = 0;
}

static int receive_coordinator_opcode(int coordinator_fd) {

	int opcode;
	if(recv(coordinator_fd, &opcode, sizeof(int), MSG_WAITALL) == -1) {

		log_error(logger, "Communication failure with coordinator in receive_coordinator_opcode");
	    exit_gracefully(EXIT_FAILURE); //TODO -- Si el coordinador murió horrendamente, qué hacemos?
	}
	return opcode;
}

static char* receive_blocked_key(int coordinator_fd) {

	package_t* package = receive_package(coordinator_fd);

	if(package == NULL) {

		log_error(logger, "Communication failure with coordinator in receive_blocked_key");
	    exit_gracefully(EXIT_FAILURE); //TODO -- Si el coordinador murió horrendamente, qué hacemos?
	}

	char* key_blocked = malloc(package->size - sizeof(int));
	memcpy(key_blocked,package->load+sizeof(int),package->size - sizeof(int));
	destroy_package(package);

	return key_blocked;
}

static void add_new_blocked_key(char* blocked_key) {

	key_blocker* key_blocker_ = malloc(sizeof(key_blocker));
	memcpy(key_blocker_->key, blocked_key, strlen(blocked_key));
	memcpy(&(key_blocker_->esi_id), (int*)g_execution_queue->head->data, sizeof(int));

	free(blocked_key);
	list_add(g_locked_keys, (void*)key_blocker_);
}

static void send_key_status(int coordinator_fd, protocol_id protocol) {

	if (send(coordinator_fd, &protocol, sizeof(protocol), 0) == -1) {

		log_error(logger, "Failure in sending key status to coordinator");
	}
}

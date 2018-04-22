#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#include "../libs/conector.h"
#include "planificador.h"

#define MAXCONN 20

// Data Structures

typedef enum { SJFCD, SJFSD, HRRN } t_scheduling_algorithm;

typedef struct {
	int port;
	int coordinator_port;
	t_scheduling_algorithm scheduling_algorithm;
	double initial_estimation;
	char* coordinator_ip;
	char** blocked_keys;
} t_planificador_config;

// Local function prototypes
static char *_string_join(char **string_array, char *separator);

// Global variables

t_config* config;

t_planificador_config setup;

t_log* logger;

t_list* g_locked_keys;
t_list* g_esi_bursts;

t_list* g_ready_queue;
t_list* g_execution_queue;
t_list* g_blocked_queue;
t_list* g_clav_bloqued_by_consol;

int g_esi_fd; /* TODO -- Arreglar cuanto antes-- es solo para buscar en una lista por fd */

int main(void) {

	init_log();

	init_config();

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
	int flag = 1;

	while(1)
	{
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

						int new_client_fd = accept(listener, (struct sockaddr *) &client_info, &addrlen);

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
								log_error(logger, "Handshake fail with socket %d", new_client_fd);
								close(new_client_fd);
							} else {

								client_confirmation = true;
								send_confirmation(new_client_fd, confirmation);
							}

							list_add(g_esi_bursts, create_esi_information(new_client_fd));
							put_esi_on_ready_queue(new_client_fd);

							if(alg_is_non_preemptive()) flag = 1;
						}

					} else if(fd == coordinator_fd){

						/* TODO -- El coordinador se comunica con el planificador*/
								//si se pregunta por clave bloqueada, se contesta y nunca se replanifica
								//si se avisa el desbloqueo, se desbloquea y si es con desalojo flag de replanificar en 1
					} else if (fd == g_execution_queue->head->data) {

						int confirmation = receive_confirmation(fd);
						if(confirmation) {

							list_iterate(g_esi_bursts, update_waited_bursts);
							//sumarle 1 al burst del esi en ejecucion
							//restarle 1 a la estimacion actual
							//revisar flag de replanificacion y replanificar si es el caso
						} else {

                            //replanificar siempre porque el esi está bloqueado

						}

					}
				}

        if(flag == 1) {
		g_esi_fd = schedule_esis();
		list_add(g_execution_queue, &g_esi_fd); //Agrega al esi elegido a la lista de ejecucion
		list_remove_and_destroy_by_condition(g_ready_queue, fd_searcher, destroy_int); //Saca de ready al esi elegido
		//SETEAR en 0 last burst del esi a ejecutar
		flag = 0;
		authorize_esi_execution(g_esi_fd);
        }
	}

	return EXIT_SUCCESS;
}

void remove_fd(int fd, fd_set *fdset)
{
	FD_CLR(fd, fdset);
	log_info(logger, "Socket %d kicked out", fd);
	close(fd);
}

key_blocker* create_key_blocker(char* key, int esi_id)
{
    key_blocker* key_blocker = malloc(sizeof(key_blocker));
    key_blocker->key = key;
    key_blocker->esi_id = esi_id;
    return key_blocker;
}

void destroy_key_blocker(void* key_blocker_)
{
	key_blocker* kb = malloc(sizeof(struct key_blocker));
	kb = (key_blocker*)key_blocker_;
	free(kb->key);
	free(kb);
	free(key_blocker_);
}

esi_information* create_esi_information(int esi_id)
{
	esi_information* esi_inf = malloc(sizeof(esi_information));
	esi_inf->esi_id = esi_id;
	esi_inf->next_left_estimated_burst = (double)setup.initial_estimation;
	esi_inf->last_estimated_burst = esi_inf->next_left_estimated_burst;
	esi_inf->last_real_burst = 0;
	esi_inf->waited_bursts = 0;
	return esi_inf;
}

void destroy_esi_information(void* esi_inf)
{
    free(esi_inf);
}

void create_administrative_structures()
{
	g_locked_keys = list_create();
	g_esi_bursts = list_create();
}

void destroy_administrative_structures()
{
	list_destroy_and_destroy_elements(g_locked_keys, destroy_key_blocker);
	list_destroy_and_destroy_elements(g_esi_bursts, destroy_esi_information);
}

void init_config() {
	config = config_create("planificador.cfg");
	log_info(logger, "Se abrio el archivo de configuracion.");

	check_config("PUERTO");
	setup.port = config_get_int_value(config, "PUERTO");
	log_info(logger, "Asignando puerto %d.", setup.port);

	check_config("ALGORITMO_PLANIFICACION");
	char* algorithm_name = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	set_distribution(algorithm_name);
	log_info(logger, "Asignado algoritmo de reemplazo de planificacion %s.", algorithm_name);
	free(algorithm_name);

	check_config("ESTIMACION_INICIAL");
	setup.initial_estimation = config_get_double_value(config, "ESTIMACION_INICIAL");
	log_info(logger, "Asignando estimacion inicial %f.", setup.initial_estimation);

	check_config("IP_COORDINADOR");
	setup.coordinator_ip = config_get_string_value(config, "IP_COORDINADOR");
	log_info(logger, "Asignando direccion coordinador %s.", setup.coordinator_ip);

	check_config("PUERTO_COORDINADOR");
	setup.coordinator_port = config_get_int_value(config, "PUERTO_COORDINADOR");
	log_info(logger, "Asignando puerto coordinador %d.", setup.coordinator_port);

	check_config("CLAVES_BLOQUEADAS");
	setup.blocked_keys = config_get_array_value(config, "CLAVES_BLOQUEADAS");

	char *key_names_str = _string_join(setup.blocked_keys, ", ");
	log_info(logger, "Asignando claves inicialmente bloqueadas [%s].", key_names_str);
	free(key_names_str);

	log_info(logger, "Se configuro el planificador correctamente.");
}

static char *_string_join(char **string_array, char *separator)
{
	char *str = string_new();
	int i;
	for (i = 0; string_array[i] != NULL; i++) {
		string_append(&str, string_array[i]);

		if (string_array[i + 1] != NULL) {
			string_append(&str, separator);
		} else {
			return str;
		}
	}

	return str;
}

void set_distribution(char* algorithm_name) {

	if(string_equals_ignore_case(algorithm_name, "SJFCD")) {
		setup.scheduling_algorithm = SJFCD;
	}
	else if(string_equals_ignore_case(algorithm_name, "SJFSD")) {
		setup.scheduling_algorithm = SJFSD;
	}
	else if(string_equals_ignore_case(algorithm_name, "HRRN")){
		setup.scheduling_algorithm = HRRN;
	}
	else {
		log_error(logger, "Se intento asignar un algoritmo inexistente llamado %s.", algorithm_name);
		exit_gracefully(EXIT_FAILURE);
	}
}
void init_log() {

	logger = log_create("planificador.log", "planificador", 1 , LOG_LEVEL_INFO);
	log_info(logger, "Logger created");
}

void check_config(char* key) {
	if(!config_has_property(config, key)) {
		log_error(logger, "No existe la clave '%s' en el archivo de configuracion.", key);

		exit_gracefully(EXIT_FAILURE);
	}
}

void exit_gracefully(int status) {

	log_info(logger, "Scheduler execution ended");

	config_destroy(config);

	log_destroy(logger);

	destroy_administrative_structures();

	exit(status);
}

void put_esi_on_ready_queue(new_client_fd) {

	list_add(g_ready_queue, &new_client_fd);
}

void update_waited_bursts(esi_information* esi_inf) {

	esi_inf->waited_bursts++;
}

void authorize_esi_execution(int esi_fd) {

	int operation_id = execute_order;
	send(esi_fd, &operation_id, sizeof(operation_id), 0);
}

int receive_confirmation_from_esi(int fd) {

	int* buffer;
	if(recv(fd, buffer, sizeof(int), MSG_WAITALL) == -1) {

		log_error(logger, "Confirmation failed");
		//no sabes hablar, sock my port -- matar esi --no hay tiempo para vos sorete esi
	}
	return *buffer;
}

void we_must_reschedule(int* flag) {

	*flag = 1;
}

bool alg_is_non_preemptive() {

	int algorithm_type = setup.scheduling_algorithm;
	switch(algorithm_type) {

	case 0: return true;
	        break;
	default: return false;
	        break;
	}
}

void destroy_int(int* int_) {

	free(int_);
}

bool fd_searcher(int* fd) {

	return *fd == g_esi_fd;

}

bool comparar_t_nodo_con_fd(esi_information* t_nodo, int fd){
if (t_nodo->esi_id == fd){
	return true;
}
else{
	return false;
}
}

esi_information* Obetener_esi_inf_segun_fd(t_list* lista, int fd){
	t_list* pr=lista;
	esi_information* pd=lista->head->data;
 bool respuesta= obtener_t_nodo_con_fd(pd,fd);
 while (respuesta){
	 pr=pr->head->next;
	 pd=pr->head->data;
	 respuesta= obtener_t_nodo_con_fd(pd,fd);
 }
 return pd;
 }





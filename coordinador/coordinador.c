#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "../libs/conector.h"

// Data Structures

typedef enum { LSU, EL, KE } t_distribution_algorithm;

// Global variables

// Configuration

t_config* config;

int port;
t_distribution_algorithm distribution;
int entries_num;
int entries_size;
int delay;

// Logger

t_log* logger;

// Implementation

void set_distribution(char* algorithm_name) {
	if(string_equals_ignore_case(algorithm_name, "LSU")) {
		distribution = LSU;
	}
	else if(string_equals_ignore_case(algorithm_name, "EL")) {
		distribution = EL;
	}
	else {
		distribution = KE;
	}
}

void init_log() {
	logger = log_create("coordinador.log", "coordinador", true, LOG_LEVEL_INFO);
}

void exit_gracefully(int status) {
	log_destroy(logger);

	config_destroy(config);

	exit(status);
}

void check_config(char* key) {
	if(!config_has_property(config, key)) {
		log_error(logger, "No existe la clave %s.", key);

		exit_gracefully(EXIT_FAILURE);
	}
}

void init_config() {
	config = config_create("./coordinador.cfg");

	check_config("PUERTO");
	port = config_get_int_value(config, "PUERTO");
	log_info(logger, "Asignado valor %d al puerto.", port);

	check_config("ALGORITMO_DISTRIBUCION");
	char* algorithm_name = config_get_string_value(config, "ALGORITMO_DISTRIBUCION");
	set_distribution(algorithm_name);
	log_info(logger, "Asignado algoritmo %s.", algorithm_name);
	free(algorithm_name);

	check_config("CANTIDAD_ENTRADAS");
	entries_num = config_get_int_value(config, "CANTIDAD_ENTRADAS");
	log_info(logger, "Asignada la cantidad de entradas a %d", entries_num);

	check_config("TAMANIO_ENTRADA");
	entries_size = config_get_int_value(config, "TAMANIO_ENTRADA");
	log_info(logger, "Asignado el tamanio de entradas a %d.", entries_size);

	check_config("RETARDO");
	delay = config_get_int_value(config, "RETARDO");
	log_info(logger, "Asignado el tiempo de retardo a %d.", delay);

	log_info(logger, "Se configur{o el Coordinador correctamente.");
}

void init_server(int port) {
	int listener_fd = init_listener(port, 10);
	if (listener_fd < 0) {
		switch (listener_fd) {
		case NO_FD_ERROR:
			log_error(logger, "No hay file descriptor disponible para el listener.");
			break;
		case BIND_ERROR:
			log_error(logger, "Error al intentar bindear.");
			break;
		case LISTEN_ERROR:
			log_error(logger, "Error al intentar crear cola de escucha.");
			break;
		}
		exit_gracefully(EXIT_FAILURE);
	} else {
		struct sockaddr_in client_info;
		socklen_t addrlen = sizeof client_info;

		pthread_attr_t attrs;
		pthread_attr_init(&attrs);
		pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

		for (;;) {
			int *accepted_fd = malloc(sizeof(accepted_fd));
			*accepted_fd = accept(listener_fd, (struct sockaddr *) &client_info, &addrlen);

			pthread_t tid;
			pthread_create(&tid, &attrs, handle_connection, accepted_fd);
		}

		pthread_attr_destroy(&attrs);
	}
}

void *handle_connection(void *arg) {
	int fd = *((int *)arg);
	free(arg);

	int type = receive_handshake(fd);
	switch (type) {
	case SCHEDULER:
		// handle_scheduler_connection();
		break;
	case ESI:
		// handle_esi_connection();
		break;
	case INSTANCE:
		// handle_instance_connection();
		break;
	default:
		// fruta();
		return NULL;
	}

	return NULL;
}

int main(void) {
	init_log();

	init_config();

	exit_gracefully(EXIT_SUCCESS);
}

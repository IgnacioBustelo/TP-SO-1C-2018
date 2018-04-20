#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "coordinador.h"
#include "../libs/conector.h"

// Data Structures

typedef enum { LSU, EL, KE } t_distribution_algorithm;

typedef struct {
	int port;
	t_distribution_algorithm distribution;
	int entries_num;
	int entries_size;
	int delay;
} t_coordinador_config;

// Global variables

t_config* config;

t_coordinador_config setup;

t_log* logger;

// Implementation

void exit_gracefully(int status) {
	log_info(logger, "Finalizo la ejecucion del Coordinador.");

	config_destroy(config);

	log_destroy(logger);

	exit(status);
}

void check_config(char* key) {
	if(!config_has_property(config, key)) {
		log_error(logger, "No existe la clave '%s' en el archivo de configuracion.", key);

		exit_gracefully(EXIT_FAILURE);
	}
}

void set_distribution(char* algorithm_name) {
	if(string_equals_ignore_case(algorithm_name, "LSU")) {
		setup.distribution = LSU;
	}
	else if(string_equals_ignore_case(algorithm_name, "EL")) {
		setup.distribution = EL;
	}
	else if(string_equals_ignore_case(algorithm_name, "KE")){
		setup.distribution = KE;
	}
	else {
		log_error(logger, "Se intento asignar un algoritmo inexistente llamado %s.", algorithm_name);
		exit_gracefully(EXIT_FAILURE);
	}
}

void init_log() {
	logger = log_create("coordinador.log", "Coordinador", true, LOG_LEVEL_INFO);
	log_info(logger, "Se inicio el logger.");
}

void init_config() {
	config = config_create("coordinador.cfg");
	log_info(logger, "Se abrio el archivo de configuracion.");

	check_config("PUERTO");
	setup.port = config_get_int_value(config, "PUERTO");
	log_info(logger, "Asignado valor %d al puerto.", setup.port);

	check_config("ALGORITMO_DISTRIBUCION");
	char* algorithm_name = config_get_string_value(config, "ALGORITMO_DISTRIBUCION");
	set_distribution(algorithm_name);
	log_info(logger, "Asignado algoritmo %s.", algorithm_name);

	check_config("CANTIDAD_ENTRADAS");
	setup.entries_num = config_get_int_value(config, "CANTIDAD_ENTRADAS");
	log_info(logger, "Asignada la cantidad de entradas a %d", setup.entries_num);

	check_config("TAMANIO_ENTRADA");
	setup.entries_size = config_get_int_value(config, "TAMANIO_ENTRADA");
	log_info(logger, "Asignado el tamanio de entradas a %d bytes.", setup.entries_size);

	check_config("RETARDO");
	setup.delay = config_get_int_value(config, "RETARDO");
	log_info(logger, "Asignado el tiempo de retardo a %d milisegundos.", setup.delay);

	log_info(logger, "Se configuro el Coordinador correctamente.");
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

		log_info(logger, "Escuchando en el socket %d", listener_fd);

		struct sockaddr_in client_info;
		socklen_t addrlen = sizeof client_info;

		pthread_attr_t attrs;
		pthread_attr_init(&attrs);
		pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

		for (;;) {
			int *accepted_fd = malloc(sizeof(int));
			*accepted_fd = accept(listener_fd, (struct sockaddr *) &client_info, &addrlen);

			log_info(logger, "Creando un hilo para atender una conexión en el socket %d", *accepted_fd);

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

	init_server(setup.port);

	exit_gracefully(EXIT_SUCCESS);
}

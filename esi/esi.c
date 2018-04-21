#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libs/conector.h"
#include "esi.h"

// Data Structures

typedef struct {
	char* ip_coordinador;
	int port_coordinador;
	char* ip_planificador;
	int port_planificador;
} t_esi_config;

// Global variables

t_config* config;

t_esi_config setup;

t_log* logger;

void exit_gracefully(int status) {
	log_info(logger, "Finalizo la ejecucion del ESI.");

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

void init_log() {
	logger = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
	log_info(logger, "Se inicio el logger.");
}

void init_config() {
	config = config_create("esi.cfg");
	log_info(logger, "Se abrio el archivo de configuracion.");

	check_config("IP_COORDINADOR");
	setup.ip_coordinador = config_get_string_value(config, "IP_COORDINADOR");
	log_info(logger, "Asignado valor %s al IP del Coordinador.", setup.ip_coordinador);

	check_config("PUERTO_COORDINADOR");
	setup.port_coordinador = config_get_int_value(config, "PUERTO_COORDINADOR");
	log_info(logger, "Asignado valor %d al puerto del Coordinador.", setup.port_coordinador);

	check_config("IP_PLANIFICADOR");
	setup.ip_planificador = config_get_string_value(config, "IP_PLANIFICADOR");
	log_info(logger, "Asignado valor %s al IP del Planificador.", setup.ip_planificador);

	check_config("PUERTO_PLANIFICADOR");
	setup.port_planificador = config_get_int_value(config, "PUERTO_PLANIFICADOR");
	log_info(logger, "Asignado valor %d al puerto del Planificador.", setup.port_planificador);

	log_info(logger, "Se configuro el ESI correctamente.");
}int port_scheduler;

int main(void) {
	init_log();

	init_config();

//Creasignacion de las variables presentes en el archivo de configuracion

	char* ip_coordinador = setup.ip_coordinador;
	char* ip_planificador=setup.ip_planificador;
	int port_coordinator=setup.port_coordinador;
	int port_scheduler=setup.port_planificador;

	bool confirmation;

    //Primera conexion, al coordinador:

	int coordinator_fd = connect_to_server(ip_coordinador, port_coordinator);
		if (send_handshake(coordinator_fd, ESI) != 1) {
			log_error(logger, "Failure in send_handshake with coordinator");
			close(coordinator_fd);
		}

	int received = receive_confirmation(coordinator_fd, &confirmation);
		if (!received || !confirmation) {
			log_error(logger, "Failure in confirmation reception from coordinator");
			close(coordinator_fd);
		}

    //Segunda conexion, al planificador:

	coordinator_fd = connect_to_server(ip_planificador, port_scheduler);
		if (send_handshake(coordinator_fd, ESI) != 1) {
			log_error(logger, "Failure in send_handshake with scheduler");
			close(coordinator_fd);
		}


	received = receive_confirmation(coordinator_fd, &confirmation);
		if (!received || !confirmation) {
			log_error(logger, "Failure in confirmation reception from scheduler");
			close(coordinator_fd);
		}

	while(1);

	exit_gracefully(EXIT_SUCCESS);
}

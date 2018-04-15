#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

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
}

int main(void) {
	init_log();

	init_config();

	exit_gracefully(EXIT_SUCCESS);
}

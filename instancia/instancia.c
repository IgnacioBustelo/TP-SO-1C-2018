#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libs/conector.h"
#include "instancia.h"

// Data Structures

typedef enum { CIRC, LRU, BSU } t_page_replacement_algorithm;

typedef struct {
	char* coordinator_ip;
	int coordinator_port;
	t_page_replacement_algorithm page_replacement_algorithm;
	char* mount_point;
	char* instance_name;
	int dump_interval;
} t_instancia_config;

// Global variables

t_config* config;

t_instancia_config setup;

t_log* logger;

int main(void) {

	init_log();

	init_config();

	char* ip_coordinador = setup.coordinator_ip;
	int port_coordinator = setup.coordinator_port;

//Conexion al coordinador

	int coordinator_fd = connect_to_server(ip_coordinador, port_coordinator);
		if (send_handshake(coordinator_fd, ESI) != 1) {
			log_error(logger, "Failure in send_handshake");
			close(coordinator_fd);
		}

		bool confirmation;
	int received = receive_confirmation(coordinator_fd, &confirmation);
		if (!received || !confirmation) {
			log_error(logger, "Failure in confirmation reception");
			close(coordinator_fd);
		}

	log_info(logger, "Conectado al coordinador");

	while(1);

	return EXIT_SUCCESS;
}

void init_log() {
	logger = log_create("instancia.log", "Instancia", true, LOG_LEVEL_INFO);
	log_info(logger, "Se inicio el logger");
}

void init_config() {

	config = config_create("instancia.cfg");
	log_info(logger, "Se abrio el archivo de configuracion.");

	check_config("IP_COORDINADOR");
	setup.coordinator_ip=config_get_string_value(config, "IP_COORDINADOR");
	log_info(logger, "Asignando direccion coordinador %s.", setup.coordinator_ip);

	check_config("PUERTO_COORDINADOR");
	setup.coordinator_port = config_get_int_value(config, "PUERTO_COORDINADOR");
	log_info(logger, "Asignando puerto coordinador %d.", setup.coordinator_port);

	check_config("ALGORITMO_REEMPLAZO");
	char* algorithm_name = config_get_string_value(config, "ALGORITMO_REEMPLAZO");
	set_distribution(algorithm_name);
	log_info(logger, "Asignado algoritmo de reemplazo de paginas %s.", algorithm_name);

	check_config("PUNTO_MONTAJE");
	setup.mount_point=config_get_string_value(config, "PUNTO_MONTAJE");
	log_info(logger, "Asignando punto de montaje %s.", setup.mount_point);

	check_config("NOMBRE_INSTANCIA");
	setup.instance_name=config_get_string_value(config, "NOMBRE_INSTANCIA");
	log_info(logger, "Asignando nombre de instancia %s.", setup.instance_name);

	check_config("INTERVALO_DUMP");
	setup.dump_interval = config_get_int_value(config, "INTERVALO_DUMP");
	log_info(logger, "Asignando intervalo de dump %d.", setup.dump_interval);

	log_info(logger, "Se configuro la %s correctamente.",setup.instance_name);
}

void set_distribution(char* algorithm_name) {

	if(string_equals_ignore_case(algorithm_name, "CIRC")) {
		setup.page_replacement_algorithm = CIRC;
	}
	else if(string_equals_ignore_case(algorithm_name, "LRU")) {
		setup.page_replacement_algorithm = LRU;
	}
	else if(string_equals_ignore_case(algorithm_name, "BSU")){
		setup.page_replacement_algorithm = BSU;
	}
	else {
		log_error(logger, "Se intento asignar un algoritmo inexistente llamado %s.", algorithm_name);
		exit_gracefully(EXIT_FAILURE);
	}
}

void check_config(char* key) {
	if(!config_has_property(config, key)) {
		log_error(logger, "No existe la clave '%s' en el archivo de configuracion.", key);

		exit_gracefully(EXIT_FAILURE);
	}
}

void exit_gracefully(int status) {
	log_info(logger, "Finalizo la ejecucion de la instancia");

	config_destroy(config);

	log_destroy(logger);

	exit(status);
}

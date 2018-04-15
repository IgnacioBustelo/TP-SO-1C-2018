#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

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
	config = config_create("coordinador.cfg");

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

int main(void) {
	init_log();

	init_config();

	exit_gracefully(EXIT_SUCCESS);
}

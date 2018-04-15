#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

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

t_coordinador_config* setup;

t_log* logger;

// Implementation

void exit_gracefully(int status) {
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
		setup->distribution = LSU;
	}
	else if(string_equals_ignore_case(algorithm_name, "EL")) {
		setup->distribution = EL;
	}
	else if(string_equals_ignore_case(algorithm_name, "KE")){
		setup->distribution = KE;
	}
	else {
		log_error(logger, "Se intentó asignar un algoritmo inexistente llamado %s.", algorithm_name);
		free(algorithm_name);
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
	setup->port = config_get_int_value(config, "PUERTO");
	log_info(logger, "Asignado valor %d al puerto.", setup->port);

	check_config("ALGORITMO_DISTRIBUCION");
	char* algorithm_name = config_get_string_value(config, "ALGORITMO_DISTRIBUCION");
	set_distribution(algorithm_name);
	log_info(logger, "Asignado algoritmo %s.", algorithm_name);
	free(algorithm_name);

	check_config("CANTIDAD_ENTRADAS");
	setup->entries_num = config_get_int_value(config, "CANTIDAD_ENTRADAS");
	log_info(logger, "Asignada la cantidad de entradas a %d", setup->entries_num);

	check_config("TAMANIO_ENTRADA");
	setup->entries_size = config_get_int_value(config, "TAMANIO_ENTRADA");
	log_info(logger, "Asignado el tamanio de entradas a %d.", setup->entries_size);

	check_config("RETARDO");
	setup->delay = config_get_int_value(config, "RETARDO");
	log_info(logger, "Asignado el tiempo de retardo a %d.", setup->delay);

	log_info(logger, "Se configuro el Coordinador correctamente.");
}

int main(void) {
	init_log();

	init_config();

	exit_gracefully(EXIT_SUCCESS);
}

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdlib.h>

#include "config.h"

#include "../libs/conector.h"
#include "../libs/handshaker.h"
#include "instancia.h"

static void check_config(t_config* config, char* key, t_log* logger) {
	if(!config_has_property(config, key)) {
		log_error(logger, "No existe la clave '%s' en el archivo de configuracion.", key);

		config_destroy(config);

		exit_gracefully(EXIT_FAILURE);
	}
}

static void set_distribution(page_replacement_algorithm_t* algorithm, char* algorithm_name, t_log* logger) {

	if(string_equals_ignore_case(algorithm_name, "CIRC")) {
		*algorithm = CIRC;
	}
	else if(string_equals_ignore_case(algorithm_name, "LRU")) {
		*algorithm = LRU;
	}
	else if(string_equals_ignore_case(algorithm_name, "BSU")){
		*algorithm = BSU;
	}
	else {
		log_error(logger, "Se intento asignar un algoritmo inexistente llamado %s.", algorithm_name);
		exit_gracefully(EXIT_FAILURE);
	}
}

t_log* init_log() {
	t_log* logger;

	logger = log_create("instancia.log", "Instancia", true, LOG_LEVEL_INFO);
	log_info(logger, "Se inicio el logger");

	return logger;
}

setup_t init_config(t_log* logger) {
	t_config* config;
	setup_t setup;

	char* keys[6] = {"IP_COORDINADOR", "PUERTO_COORDINADOR", "ALGORITMO_REEMPLAZO", "PUNTO_MONTAJE", "NOMBRE_INSTANCIA", "INTERVALO_DUMP"};

	config = config_create("instancia.cfg");
	log_info(logger, "Se abrio el archivo de configuracion.");

	check_config(config, keys[0], logger);
	setup.coordinator_ip = strdup(config_get_string_value(config, keys[0]));
	log_info(logger, "Asignando direccion coordinador %s.", setup.coordinator_ip);

	check_config(config, keys[1], logger);
	setup.coordinator_port = config_get_int_value(config, keys[1]);
	log_info(logger, "Asignando puerto coordinador %d.", setup.coordinator_port);

	check_config(config, keys[2], logger);
	set_distribution(&(setup.page_replacement_algorithm), config_get_string_value(config, keys[2]), logger);
	log_info(logger, "Asignado algoritmo de reemplazo de paginas %s.", config_get_string_value(config, keys[2]));

	check_config(config, keys[3], logger);
	setup.mount_point = strdup(config_get_string_value(config, keys[3]));
	log_info(logger, "Asignando punto de montaje %s.", setup.mount_point);

	check_config(config, keys[4], logger);
	setup.instance_name = strdup(config_get_string_value(config, keys[4]));
	log_info(logger, "Asignando nombre de instancia %s.", setup.instance_name);

	check_config(config, keys[5], logger);
	setup.dump_interval = config_get_int_value(config, keys[5]);
	log_info(logger, "Asignando intervalo de dump %d.", setup.dump_interval);

	log_info(logger, "Se configuro la %s correctamente.", setup.instance_name);

	config_destroy(config);

	return setup;
}

void handshake_coordinador(int coordinador_fd, t_log* logger) {
	int status;

	status = handshake_client(coordinador_fd, "Coordinador", INSTANCE, logger);

	if(status != EXIT_SUCCESS) {
		exit_gracefully(status);
	}
}

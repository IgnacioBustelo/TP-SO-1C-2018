#include <string.h>

#include "config.h"

/* -- Local function prototypes  -- */

static void check_config(t_log* logger,t_config* config, char* key);
static void set_distribution(t_log* logger, t_planificador_config* setup, char* algorithm_name);
static void verify_alpha(t_log* logger, t_planificador_config *setup, int alpha);
static char *_string_join(char **string_array, char *separator);

t_log* init_log() {

	t_log* logger = log_create("planificador.log", "planificador", 1 , LOG_LEVEL_INFO);
	log_info(logger, "Logger created");
	return logger;
}

t_planificador_config init_config(t_log* logger) {

	t_config* config;
	t_planificador_config setup;

	config = config_create("planificador.cfg");
	log_info(logger, "Se abrio el archivo de configuracion.");

	check_config(logger, config, "PUERTO");
	setup.port = config_get_int_value(config, "PUERTO");
	log_info(logger, "Asignando puerto %d.", setup.port);

	check_config(logger, config,"ALGORITMO_PLANIFICACION");
	char* algorithm_name = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	set_distribution(logger, &setup, algorithm_name);
	log_info(logger, "Asignado algoritmo de reemplazo de planificacion %s.", algorithm_name);

	check_config(logger, config, "ALPHA");
	int alpha = config_get_int_value(config, "ALPHA");
	verify_alpha(logger, &setup, alpha);
	log_info(logger, "Asignando alpha %f.", setup.alpha);

	check_config(logger, config,"ESTIMACION_INICIAL");
	setup.initial_estimation = config_get_double_value(config, "ESTIMACION_INICIAL");
	log_info(logger, "Asignando estimacion inicial %f.", setup.initial_estimation);

	check_config(logger, config,"IP_COORDINADOR");
	setup.coordinator_ip = strdup(config_get_string_value(config, "IP_COORDINADOR"));
	log_info(logger, "Asignando direccion coordinador %s.", setup.coordinator_ip);

	check_config(logger, config,"PUERTO_COORDINADOR");
	setup.coordinator_port = config_get_int_value(config, "PUERTO_COORDINADOR");
	log_info(logger, "Asignando puerto coordinador %d.", setup.coordinator_port);

	check_config(logger, config,"CLAVES_BLOQUEADAS");
	setup.blocked_keys = config_get_array_value(config, "CLAVES_BLOQUEADAS");

	char *key_names_str = _string_join(setup.blocked_keys, ", ");
	log_info(logger, "Asignando claves inicialmente bloqueadas [%s].", key_names_str);
	free(key_names_str);

	log_info(logger, "Se configuro el planificador correctamente.");

	config_destroy(config);

	return setup;
}

/* --- PRIVATE FUNCTIONS --- */

static void check_config(t_log* logger,t_config* config, char* key) {
	if(!config_has_property(config, key)) {
		log_error(logger, "No existe la clave '%s' en el archivo de configuracion.", key);

		exit_gracefully(EXIT_FAILURE);
	}
}

static void set_distribution(t_log* logger, t_planificador_config* setup, char* algorithm_name) {

	if(string_equals_ignore_case(algorithm_name, "SJFCD")) {
		setup->scheduling_algorithm = SJFCD;
	}
	else if(string_equals_ignore_case(algorithm_name, "SJFSD")) {
		setup->scheduling_algorithm = SJFSD;
	}
	else if(string_equals_ignore_case(algorithm_name, "HRRN")){
		setup->scheduling_algorithm = HRRN;
	}
	else if(string_equals_ignore_case(algorithm_name, "FIFO")) {
		setup->scheduling_algorithm = FIFO;
	} else {
		log_error(logger, "Se intento asignar un algoritmo inexistente llamado %s.", algorithm_name);
		exit_gracefully(EXIT_FAILURE);
	}
}

static void verify_alpha(t_log* logger, t_planificador_config *setup, int alpha) {

	if (alpha >= 0 && alpha <= 100) {
		setup->alpha = (float)alpha/100;
	} else {
		log_error(logger, "Se intento usar un alpha = &i y es inapropiado.", alpha/100);
		exit_gracefully(EXIT_FAILURE);
	}
}

static char *_string_join(char **string_array, char *separator) {
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

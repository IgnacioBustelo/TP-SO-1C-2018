#include <string.h>

#include "config.h"

/* -- Local function prototypes  -- */

static void check_config(t_log* logger, t_config* config, char* key);

t_log* init_log() {
	t_log* logger = log_create("esi.log", "ESI", true, LOG_LEVEL_INFO);
	log_info(logger, "Se inicio el logger.");
	return logger;
}

t_esi_config init_config(t_log* logger) {

	t_config* config;
	t_esi_config setup;

	config = config_create("esi.cfg");
	log_info(logger, "Se abrio el archivo de configuracion.");

	check_config(logger, config,"IP_COORDINADOR");
	setup.ip_coordinador = strdup(config_get_string_value(config, "IP_COORDINADOR"));
	log_info(logger, "Asignado valor %s al IP del Coordinador.", setup.ip_coordinador);

	check_config(logger, config,"PUERTO_COORDINADOR");
	setup.port_coordinador = config_get_int_value(config, "PUERTO_COORDINADOR");
	log_info(logger, "Asignado valor %d al puerto del Coordinador.", setup.port_coordinador);

	check_config(logger, config,"IP_PLANIFICADOR");
	setup.ip_planificador = strdup(config_get_string_value(config, "IP_PLANIFICADOR"));
	log_info(logger, "Asignado valor %s al IP del Planificador.", setup.ip_planificador);

	check_config(logger, config,"PUERTO_PLANIFICADOR");
	setup.port_planificador = config_get_int_value(config, "PUERTO_PLANIFICADOR");
	log_info(logger, "Asignado valor %d al puerto del Planificador.", setup.port_planificador);

	log_info(logger, "Se configuro el ESI correctamente.");

	config_destroy(config);

	return setup;
}

/* -- PRIVATE FUNCTIONS -- */

static void check_config(t_log* logger, t_config* config, char* key) {
	if(!config_has_property(config, key)) {
		log_error(logger, "No existe la clave '%s' en el archivo de configuracion.", key);

		exit_gracefully(EXIT_FAILURE);
	}
}

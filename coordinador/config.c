#include <stdbool.h>
#include <stdlib.h>

#include <commons/log.h>
#include <commons/config.h>
#include <commons/string.h>

#include "coordinador.h"
#include "config.h"

t_log *logger;
struct setup_t setup;

static void check_config(t_config *config, char* key);
static void set_distribution(char* algorithm_name);

void init_config()
{
	t_config* config = config_create("coordinador.cfg");
	log_info(logger, "Se abrio el archivo de configuracion.");

	check_config(config, "PUERTO");
	setup.port = config_get_int_value(config, "PUERTO");
	log_info(logger, "Asignado valor %d al puerto.", setup.port);

	check_config(config, "ALGORITMO_DISTRIBUCION");
	char* algorithm_name = config_get_string_value(config, "ALGORITMO_DISTRIBUCION");
	set_distribution(algorithm_name);
	log_info(logger, "Asignado algoritmo %s.", algorithm_name);

	check_config(config, "CANTIDAD_ENTRADAS");
	setup.entries_num = config_get_int_value(config, "CANTIDAD_ENTRADAS");
	log_info(logger, "Asignada la cantidad de entradas a %d", setup.entries_num);

	check_config(config, "TAMANIO_ENTRADA");
	setup.entries_size = config_get_int_value(config, "TAMANIO_ENTRADA");
	log_info(logger, "Asignado el tamanio de entradas a %d bytes.", setup.entries_size);

	check_config(config, "RETARDO");
	setup.delay = config_get_int_value(config, "RETARDO");
	log_info(logger, "Asignado el tiempo de retardo a %d milisegundos.", setup.delay);

	log_info(logger, "Se configuro el Coordinador correctamente.");

	config_destroy(config);
}

static void check_config(t_config *config, char* key)
{
	if (!config_has_property(config, key)) {
		log_error(logger, "No existe la clave '%s' en el archivo de configuracion.", key);
		exit_gracefully(EXIT_FAILURE);
	}
}

static void set_distribution(char* algorithm_name)
{
	if (string_equals_ignore_case(algorithm_name, "LSU")) {
		setup.distribution = LSU;
	} else if (string_equals_ignore_case(algorithm_name, "EL")) {
		setup.distribution = EL;
	} else if (string_equals_ignore_case(algorithm_name, "KE")) {
		setup.distribution = KE;
	} else {
		log_error(logger, "Se intento asignar un algoritmo inexistente llamado %s.", algorithm_name);
		exit_gracefully(EXIT_FAILURE);
	}
}

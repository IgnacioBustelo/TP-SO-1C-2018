#include <commons/string.h>

#include "../libs/configurator.h"
#include "cfg_instancia.h"

#define FIELD(X) (char*) list_get(config_field_list, X)

char* cfg_instancia_fields[FIELDS] = {
	"IP_COORDINADOR",
	"PUERTO_COORDINADOR",
	"ALGORITMO_REEMPLAZO",
	"PUNTO_MONTAJE",
	"NOMBRE_INSTANCIA",
	"INTERVALO_DUMP"
};

static char cfg_instancia_choose_algorithm() {
	char* algorithm = config_get_string_value(config, FIELD(2));

	if(string_equals_ignore_case(algorithm, "CIRC")) {

		return 'C'; // Circular

	} else if(string_equals_ignore_case(algorithm, "LRU")) {

		return 'L'; // Least Recently Used

	} else if(string_equals_ignore_case(algorithm, "BSU")) {

		return 'B'; // Biggest Space Used

	} else {

		return 'U'; // Unknown - Error
	}
}

void cfg_instancia_init(char* config_path) {
	configurator_init(config_path, cfg_instancia_fields, FIELDS);
}

char* cfg_instancia_get_coordinador_ip() {
	return config_get_string_value(config, FIELD(0));
}

int cfg_instancia_get_coordinador_port() {
	return config_get_int_value(config, FIELD(1));
}

char* cfg_instancia_get_replacement_algorithm_name() {
	switch (cfg_instancia_choose_algorithm()) {
		case 'C':	return "Algoritmo Circular";	break;

		case 'L':	return "Least Recently Used";	break;

		case 'B':	return "Biggest Space Used";	break;

		default:	return "Desconocido";			break;
	}
}

void cfg_instancia_invoke_replacement_algorithm() {
	switch (cfg_instancia_choose_algorithm()) {
		case 'C':	/*TODO: Desarrollar algoritmo Circular*/				break;

		case 'L':	/*TODO: Desarrollar algoritmo Least Recently Used*/		break;

		case 'B':	/*TODO: Desarrollar algoritmo Biggest Space Used*/		break;

		default:	/*TODO: Generar error*/									break;
	}
}

char* cfg_instancia_get_mount_point() {
	return config_get_string_value(config, FIELD(3));
}

char* cfg_instancia_get_instance_name() {
	return config_get_string_value(config, FIELD(4));
}

int cfg_instancia_get_dump_time() {
	return config_get_int_value(config, FIELD(5));
}

void cfg_instancia_show() {
	messenger_show("INFO", "Lectura del archivo de configuracion de la Instancia");
	messenger_show("INFO", "La IP del Coordinador es %s", cfg_instancia_get_coordinador_ip());
	messenger_show("INFO", "El puerto Coordinador es %d", cfg_instancia_get_coordinador_port());
	messenger_show("INFO", "El algoritmo de reemplazo es %s", cfg_instancia_get_replacement_algorithm_name());
	messenger_show("INFO", "El punto de montaje es %s", cfg_instancia_get_mount_point());
	messenger_show("INFO", "El nombre de la instancia es %s", cfg_instancia_get_instance_name());
	messenger_show("INFO", "El intervalo de dump es %d", cfg_instancia_get_dump_time());
}

void cfg_instancia_destroy() {
	configurator_destroy();
}

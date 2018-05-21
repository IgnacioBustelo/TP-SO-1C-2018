#include "cfg_instancia.h"

#include <commons/string.h>

#include "../libs/configurator.h"

char* cfg_instancia_fields[FIELDS] = {
	"IP_COORDINADOR",
	"PUERTO_COORDINADOR",
	"ALGORITMO_REEMPLAZO",
	"PUNTO_MONTAJE",
	"NOMBRE_INSTANCIA",
	"INTERVALO_DUMP"
};

static char cfg_instancia_choose_algorithm() {
	char* algorithm = config_get_string_value(config, cfg_instancia_fields[2]);

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
	return config_get_string_value(config, cfg_instancia_fields[0]);
}

int cfg_instancia_get_coordinador_port() {
	return config_get_int_value(config, cfg_instancia_fields[1]);
}

char* cfg_instancia_get_replacement_algorithm_name() {
	switch (cfg_instancia_choose_algorithm()) {
		case 'C':	return string_duplicate("Algoritmo Circular");			break;

		case 'L':	return string_duplicate("Least Recently Used");			break;

		case 'B':	return string_duplicate("Biggest Space Used");			break;

		default:	return string_duplicate("Desconocido");					break;
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
	return config_get_string_value(config, cfg_instancia_fields[3]);
}

char* cfg_instancia_get_instance_name() {
	return config_get_string_value(config, cfg_instancia_fields[4]);
}

int cfg_instancia_get_dump_time() {
	return config_get_int_value(config, cfg_instancia_fields[5]);
}

void cfg_instancia_destroy() {
	configurator_destroy();
}

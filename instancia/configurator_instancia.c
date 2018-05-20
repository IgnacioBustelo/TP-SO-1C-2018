#include <commons/string.h>

#include "configurator_instancia.h"

char* config_keys[6] = {
	"IP_COORDINADOR",
	"PUERTO_COORDINADOR",
	"ALGORITMO_REEMPLAZO",
	"PUNTO_MONTAJE",
	"NOMBRE_INSTANCIA",
	"INTERVALO_DUMP"
};

static char configurator_choose_algorithm() {
	char* algorithm = config_get_string_value(config, config_keys[2]);

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

char* configurator_get_coordinador_ip() {
	return config_get_string_value(config, config_keys[0]);
}

int configurator_get_coordinador_port() {
	return config_get_int_value(config, config_keys[1]);
}

char* configurator_get_replacement_algorithm_name() {
	switch (configurator_choose_algorithm()) {
		case 'C':	return string_duplicate("Algoritmo Circular");			break;

		case 'L':	return string_duplicate("Least Recently Used");			break;

		case 'B':	return string_duplicate("Biggest Space Used");			break;

		default:	return string_duplicate("Desconocido");					break;
	}
}

void configurator_invoke_replacement_algorithm() {
	switch (configurator_choose_algorithm()) {
		case 'C':	/*TODO: Desarrollar algoritmo Circular*/				break;

		case 'L':	/*TODO: Desarrollar algoritmo Least Recently Used*/		break;

		case 'B':	/*TODO: Desarrollar algoritmo Biggest Space Used*/		break;

		default:	/*TODO: Generar error*/									break;
	}
}

char* configurator_get_mount_point() {
	return config_get_string_value(config, config_keys[3]);
}

char* configurator_get_instance_name() {
	return config_get_string_value(config, config_keys[4]);
}

int configurator_get_dump_time() {
	return config_get_int_value(config, config_keys[5]);
}

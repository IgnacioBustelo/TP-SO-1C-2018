#include <commons/string.h>
#include <stdlib.h>

#include "logger.h"

static char logger_level_check(char* level) {
	if(string_equals_ignore_case(level, "TRACE")) {

		return 'T'; // Nivel de log: TRACE

	} else if(string_equals_ignore_case(level, "DEBUG")) {

		return 'D'; // Nivel de log: DEBUG

	} else if(string_equals_ignore_case(level, "INFO")) {

		return 'I'; // Nivel de log: INFO

	} else if(string_equals_ignore_case(level, "WARNING")) {

		return 'W'; // Nivel de log: WARNING

	} else if(string_equals_ignore_case(level, "ERROR")) {

		return 'E'; // Nivel de log: ERROR

	} else {

		return 'U'; // Nivel de log desconocido - posiblemente erroneo

	}
}

void messenger_init(char* logger_route, char* process_name, char* log_level) {
	if(logger_level_check(log_level) != 'U') {
		logger = log_create(logger_route, process_name, true, log_level_from_string(log_level));

		messenger_show("INFO", "Se iniciaron las operaciones de %s", process_name);
	}

	else {
		logger = log_create(logger_route, process_name, true, log_level_from_string("ERROR"));

		/* TODO: Error por fallo en el nivel de log */
	}
}

void messenger_show_method(char* log_level, char* message) {
	switch(logger_level_check(log_level)) {

		case 'T':	log_trace(logger, message);		break;

		case 'D':	log_debug(logger, message);		break;

		case 'I':	log_info(logger, message);		break;

		case 'W':	log_warning(logger, message);	break;

		case 'E':	log_error(logger, message);		break;

		default:	log_error(logger, "Nivel de log no especificado"); break;

	}
}

void messenger_destroy() {
	messenger_show("INFO", "Terminaron las operaciones de %s", logger->program_name);

	log_destroy(logger);
}

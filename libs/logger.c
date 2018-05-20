#include <commons/string.h>

#include "logger.h"

void messenger_log(char* message, char* level) {
	if(string_equals(level, "TRACE")) {

		log_trace(logger, message);

	} else if(string_equals(level, "DEBUG")) {

		log_debug(logger, message);

	} else if(string_equals(level, "INFO")) {

		log_info(logger, message);

	} else if(string_equals(level, "WARNING")) {

		log_warning(level, message);

	} else if(string_equals(level, "ERROR")) {

		log_error(logger, message);

	} else {

		log_error(logger, "No se especifico el tipo de error");

	}
}

void logger_init(char* logger_route, char* process_name, char* log_level) {
	char* message = string_from_format("Se iniciaron las operaciones de %s", process_name);

	logger = log_create(logger_route, process_name, true, log_level_from_string(log_level));

	messenger_log(message, "INFO");

	free(message);
}

void logger_destroy() {
	char* message = string_from_format("Terminaron las operaciones de %s", logger->program_name);

	messenger_log(message, "INFO");

	log_destroy(logger);

	free(message);
}

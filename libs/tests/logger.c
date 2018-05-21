#include <commons/string.h>
#include <stdlib.h>

#include "../logger.h"

static char* get_level(int i) {
	switch(i % 6) {
		case 0: return "TRACE";		break;

		case 1: return "DEBUG";		break;

		case 2: return "INFO";		break;

		case 3: return "WARNING";	break;

		case 4: return "ERROR";		break;

		default: return "RANDOM";	break;
	}
}

int main(int argc, char* argv[]) {
	char* log_level = (argc < 2) ? "UNKNOWN" : argv[1];

	logger_init("logger.log", "Logger", log_level);

	char* message =  string_from_format("Prueba de LOGGER en nivel %s para imprimir los argumentos del programa por pantalla", log_level);

	messenger_log(message, log_level);

	free(message);

	int i;
	for(i = 2; i < argc; i++) {
		char* header = string_from_format("Argumento %d = %s", i - 1, argv[i]);

		messenger_log(header, get_level(i - 1));

		free(header);
	}

	logger_destroy();
}

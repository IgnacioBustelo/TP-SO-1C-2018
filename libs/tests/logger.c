#include <commons/string.h>
#include <stdlib.h>

#include "../logger.h"

static char* get_level(int i) {
	switch(i % 6) {
		case 0:		return	"TRACE";	break;

		case 1:		return	"DEBUG";	break;

		case 2:		return	"INFO";		break;

		case 3:		return	"WARNING";	break;

		case 4:		return	"ERROR";	break;

		default:	return	"RANDOM";	break;
	}
}

int main(int argc, char* argv[]) {
	char* log_level = (argc < 2) ? "UNKNOWN" : argv[1];

	messenger_init("logger.log", "Logger", log_level);

	messenger_show(log_level, "Prueba de LOGGER en nivel %s para imprimir los argumentos del programa por pantalla", log_level);

	int i;
	for(i = 2; i < argc; i++) {

		messenger_show(get_level(i - 1), "Argumento %d = %s", i, argv[i]);

	}

	messenger_destroy();
}

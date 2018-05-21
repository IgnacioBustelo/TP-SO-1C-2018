#include <commons/string.h>
#include <stdlib.h>

#include "../mocks/printfer.h"

int main(int argc, char* argv[]) {
	messenger_log("Prueba de PRINTFER para imprimir los argumentos del programa por pantalla", "TEST");

	char *arguments = string_duplicate("Argumentos: ");

	int i;
	for(i = 1; i < argc; i++) {
		char* arg = string_duplicate(argv[i]);
		char* comma = string_duplicate((i != argc - 1) ? ", " : "");

		string_append(&arguments, arg);
		string_append(&arguments, comma);

		free(arg);
		free(comma);
	}

	messenger_log(arguments, "ARGS");

	free(arguments);
}

#include <commons/string.h>
#include <stdlib.h>

#include "../configurator.h"
#include "../mocks/printfer.h"

int main(int argc, char* argv[]) {
	if(argc == 1) {
		messenger_log("Inserte una lista de campos que tiene el archivo de configuracion", "ERROR");
		exit(EXIT_FAILURE);
	}

	int i, size = argc - 1;
	char** fields = calloc(size, sizeof(char*));

	for(i = 0; i < size; i++) {
		fields[i] = string_duplicate(argv[i + 1]);
	}

	configurator_init("config.cfg", fields, size);

	configurator_read();

	configurator_destroy();

	for(i = 0; i < size; i++) {
		free(fields[i]);
	}

	free(fields);
}

#include <commons/string.h>
#include <stdio.h>
#include <string.h>

#include "../storage.h"
#include "utils.h"

int main(int argc, char* argv[]) {
	storage_setup.total_entries = (argc < 2) ? 10 : (size_t) atoi(argv[1]);

	printf("Cantidad de entradas: %d\n", storage_setup.total_entries);

	storage_init();

	printf("Valores almacenados en el Storage:\n");
	print_ordered_stored_values();

	dictionary_destroy_and_destroy_elements(storage, free);

	exit(EXIT_SUCCESS);
}

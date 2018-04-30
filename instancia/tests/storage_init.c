#include <commons/string.h>
#include <stdio.h>
#include <string.h>

#include "../storage.h"

// Utilidades

static void print_values(char* key, char* value) {
	printf("Entrada %s:\t", key);

	if(string_length(value) == 0) {
		printf("NULL\n");
	}

	else {
		printf("%s\n", value);
	}
}

// Implementaciones mock

int storage_next_entry(size_t key_size) {
	return 1;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		total_entries = 10;
	}

	else {
		total_entries = (size_t) atoi(argv[1]);
	}

	storage_init();

	dictionary_iterator(storage, (void*) print_values);

	dictionary_destroy_and_destroy_elements(storage, free);

	exit(EXIT_SUCCESS);
}

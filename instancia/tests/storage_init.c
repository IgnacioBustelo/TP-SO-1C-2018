#include <stdio.h>
#include <string.h>

#include "../storage.h"

static void print_values(char* key, void* value) {
	printf("Entrada %s: ", key);

	if(value == NULL) {
		printf("NULL\n");
	}

	else {
		printf("%s\n", (char*) value);
	}
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

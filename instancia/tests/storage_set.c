#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>

#include "../storage.h"
#include "utils.h"

// Implementaciones mock

int storage_next_entry(size_t size) {
	static int next_entry = 0, current_entry = 0;

	current_entry = next_entry;
	next_entry = current_entry + required_entries(size);

	return current_entry;
}

// Creación y Destrucción

t_list* key_value_list;

void before(int argc, char* argv[]) {
	key_value_list = list_create();

	total_entries = (argc < 2) ? 10 : (size_t) atoi(argv[1]);
	entry_size = (argc < 3) ? 5 : (size_t) atoi(argv[2]);

	if(argc < 5) {
		list_add(key_value_list, key_value_create("A", "XD"));
		list_add(key_value_list, key_value_create("B", "DX"));
		list_add(key_value_list, key_value_create("C", "aaaaskccbb"));
	}

	else {
		key_value_input(key_value_list, 3, argc, argv);
	}

	storage_init();
}

void after() {
	dictionary_destroy_and_destroy_elements(storage, free);

	list_destroy_and_destroy_elements(key_value_list, (void*) key_value_destroy);

	exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[]) {
	before(argc, argv);

	printf("Tamanio de entradas: %d\nCantidad de entradas: %d\n", entry_size, total_entries);

	list_iterate(key_value_list, (void*) storage_set);

	printf("Valores almacenados en el Storage:\n");
	print_ordered_stored_values();

	after();
}

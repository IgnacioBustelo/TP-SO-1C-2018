#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>

#include "../storage.h"
#include "utils.h"

// Implementaciones mock

void store_next_key_value(key_value_t* key_value) {
	static int next_entry = 0, current_entry = 0;

	current_entry = next_entry;
	next_entry = current_entry + entry_table_required_entries(key_value->size);

	storage_set(current_entry, key_value);
}

// Creación y Destrucción

t_list* key_value_list;

void before(int argc, char* argv[]) {
	key_value_list = list_create();

	storage_setup.total_entries = (argc < 2) ? 10 : (size_t) atoi(argv[1]);
	storage_setup.entry_size = (argc < 3) ? 5 : (size_t) atoi(argv[2]);

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

	printf("Tamanio de entradas: %d\nCantidad de entradas: %d\n", storage_setup.entry_size, storage_setup.total_entries);

	list_iterate(key_value_list, (void*) store_next_key_value);

	printf("Valores almacenados en el Storage:\n");
	print_ordered_stored_values();

	after();
}

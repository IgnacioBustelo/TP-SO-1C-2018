#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../entry_table.h"
#include "utils.h"

// Implementaciones mock

int entry_table_update(key_value_t* key_value) {
	return ET_INSERT_SUCCESS;
}

int storage_set(key_value_t* key_value) {
	static int next_entry = 1, current_entry = 1;

	current_entry = next_entry;
	next_entry = current_entry + required_entries(key_value->size);

	return current_entry;
}

// Creación y Destrucción

t_list* list_key_values;

static void before(int argc, char *argv[]) {
	entry_size = (argv[1] == NULL || atoi(argv[1]) < 1) ? 5 : (size_t) atoi(argv[1]);

	list_key_values = list_create();

	if(argc < 4) {
		list_add(list_key_values, (void*) value_generator("A", 2));
		list_add(list_key_values, (void*) value_generator("B", 5));
		list_add(list_key_values, (void*) value_generator("C", 7));
		list_add(list_key_values, (void*) value_generator("D", 13));
		list_add(list_key_values, (void*) value_generator("E", 15));
	}

	else {
		key_value_input(list_key_values, 2, argc, argv);
	}

	entry_table_init();
}

static void after() {
	list_destroy_and_destroy_elements(list_key_values, (void*) key_value_destroy);

	dictionary_destroy_and_destroy_elements(entry_table, free);

	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
	before(argc, argv);

	printf("Tamanio de entradas: %d\n", entry_size);

	printf("Mostrar Claves-Valor:\n");
	list_iterate(list_key_values, (void*) print_key_value);

	printf("Ingresar claves en la tabla de entradas:\n");
	list_iterate(list_key_values, (void*) entry_table_insert);

	printf("Mostrar Tabla De Entradas:\n");
	dictionary_iterator(entry_table, (void*) print_entry);

	after();
}

#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../entry_table.h"
#include "../instancia.h"

// Variables globales

t_list* list_key_values;

// Utilidades

static key_value_t* key_value_create(char* key, char* value, size_t size) {
	key_value_t* key_value = malloc(sizeof(key_value_t));

	key_value->key = string_substring_until(key, 40);
	key_value->value = string_duplicate(value);
	key_value->size = size;

	return key_value;
}

static void key_value_destroy(key_value_t* key_value) {
	free(key_value->key);
	free(key_value->value);
	free(key_value);
}

static key_value_t* value_generator(char key[40], size_t size) {
	char* value = string_repeat('X', size);

	key_value_t* key_value = key_value_create(key, value, string_length(value));

	free(value);

	return key_value;
}

static int required_entries(size_t size) {
	return size < entry_size ? 1 : size/entry_size;
}

static void print_key_value(key_value_t* key_value) {
	printf("Clave: %s, Valor: %s, Tamanio: %d\n", key_value->key, key_value->value, key_value->size);
}

static void print_entry(char* key, entry_t* entry) {
	printf("Clave: %s, Entrada: %d, Tamanio: %d, Ocupa %d entrada/s\n", key, entry->number, entry->size, required_entries(entry->size));
}

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

static void before(int argc, char *argv[]) {
	if(argv[1] == NULL) {
		entry_size = 5;
	}

	else {
		entry_size = (size_t) atoi(argv[1]);
	}

	list_key_values = list_create();

	if(argc < 4) {
		list_add(list_key_values, (void*) value_generator("A", 2));
		list_add(list_key_values, (void*) value_generator("B", 5));
		list_add(list_key_values, (void*) value_generator("C", 7));
		list_add(list_key_values, (void*) value_generator("D", 13));
		list_add(list_key_values, (void*) value_generator("E", 15));
	}

	else {
		 int i;
		 for(i = 2; i < argc; i += 2) {
			 if(argv[i + 1] != NULL) {
				 list_add(list_key_values, key_value_create(argv[i], argv[i + 1], string_length(argv[i + 1])));
			 }
		 }
	}

	entry_table = dictionary_create();
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

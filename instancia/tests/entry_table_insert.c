#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../entry_table.h"
#include "../storage.h"

// Variables globales

t_list* list_key_values;

// Utilidades

static key_value_t* key_value_create(char* key, void* value, int size) {
	key_value_t* key_value = malloc(sizeof(key_value_t));
	key_value->value = malloc(size);

	key_value->key = string_substring_until(key, 40);
	memcpy(key_value->value, value, size);
	key_value->size = size;

	return key_value;
}

static void key_value_destroy(key_value_t* key_value) {
	free(key_value->key);
	free(key_value->value);
	free(key_value);
}

static key_value_t* value_generator(char key[40], int size) {
	char* value = string_repeat('X', size);

	key_value_t* key_value = key_value_create(key, (void*) value, string_length(value));

	free(value);

	return key_value;
}

static int required_entries(int size) {
	return size < entry_size ? 1 : size/entry_size;
}

static void print_key_value(key_value_t* key_value) {
	printf("Clave: %s, Valor: %s, Tamanio: %d\n", key_value->key, (char*) key_value->value, key_value->size);
}

static void print_entry(char* key, entry_t* entry) {
	printf("Clave: %s, Entrada: %d, Tamanio: %d, Ocupa %d entradas\n", key, entry->number, entry->size, required_entries(entry->size));
}

// Implementaciones mock

int update_entry(key_value_t* key_value) {
	return ET_INSERT_SUCCESS;
}

int set_value(key_value_t* key_value) {
	static int next_entry = 1, current_entry = 1;

	current_entry = next_entry;
	next_entry = current_entry + required_entries(key_value->size);

	return current_entry;
}

// Creación y Destrucción

static void before() {
	entry_size = 5;

	list_key_values = list_create();

	list_add(list_key_values, (void*) value_generator("1A", 2));
	list_add(list_key_values, (void*) value_generator("2B", 5));
	list_add(list_key_values, (void*) value_generator("3C", 7));
	list_add(list_key_values, (void*) value_generator("4D", 13));
	list_add(list_key_values, (void*) value_generator("5E", 15));

	entry_table = dictionary_create();
}

static void after() {
	list_destroy_and_destroy_elements(list_key_values, (void*) key_value_destroy);
	dictionary_destroy_and_destroy_elements(entry_table, free);
}

int main(void) {
	before();

	printf("Tamanio de entradas: %d\n", entry_size);

	printf("Mostrar Claves-Valor:\n");
	list_iterate(list_key_values, (void*) print_key_value);

	printf("Ingresar claves en la tabla de entradas:\n");
	list_iterate(list_key_values, (void*) insert_entry);

	printf("Mostrar Tabla De Entradas:\n");
	dictionary_iterator(entry_table, (void*) print_entry);

	after();
}

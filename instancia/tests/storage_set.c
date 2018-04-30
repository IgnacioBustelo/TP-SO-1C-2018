#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>

#include "../instancia.h"
#include "../storage.h"

// Utilidades

static key_value_t* key_value_create(char* key, char* value) {
	key_value_t* key_value = malloc(sizeof(key_value_t));

	key_value->key = string_substring_until(key, 40);
	key_value->value = string_duplicate(value);
	key_value->size = (size_t) string_length(value) + 1;

	return key_value;
}

static void key_value_destroy(key_value_t* key_value) {
	free(key_value->key);
	free(key_value->value);
	free(key_value);
}

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

static int required_entries(size_t size) {
	int required_entries = size/entry_size;

	return size % entry_size == 0 ? required_entries : ++required_entries;
}

int storage_next_entry(size_t size) {
	static int next_entry = 1, current_entry = 1;

	current_entry = next_entry;
	next_entry = current_entry + required_entries(size);

	return current_entry;
}


int main(int argc, char* argv[]) {
	if(argc < 2) {
		total_entries = 10;
		entry_size = 5;
	}

	else if(argc < 3) {
		total_entries = (size_t) atoi(argv[1]);
		entry_size = 5;
	}

	else if(argc >= 3){
		total_entries = (size_t) atoi(argv[1]);
		entry_size = (size_t) atoi(argv[2]);
	}

	printf("Tamanio de entradas: %d\nCantidad de entradas: %d\n", entry_size, total_entries);

	storage_init();

	t_list* key_value_list = list_create();

	list_add(key_value_list, key_value_create("A", "XD"));
	list_add(key_value_list, key_value_create("B", "DX"));
	list_add(key_value_list, key_value_create("C", "aaaaskccbb"));

	list_iterate(key_value_list, (void*) storage_set);

	dictionary_iterator(storage, (void*) print_values);

	dictionary_destroy_and_destroy_elements(storage, free);

	list_destroy_and_destroy_elements(key_value_list, (void*) key_value_destroy);

	exit(EXIT_SUCCESS);
}

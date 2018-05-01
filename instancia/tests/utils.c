#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../instancia.h"
#include "utils.h"

key_value_t* key_value_create(char* key, char* value) {
	key_value_t* key_value = malloc(sizeof(key_value_t));

	key_value->key = string_substring_until(key, 40);
	key_value->value = string_duplicate(value);
	key_value->size = (size_t) (string_length(value) + 1);

	return key_value;
}

void key_value_input(t_list* list_key_values, int from, int argc, char* argv[]) {
	int i;

	for(i = from; i < argc && argv[i + 1] != NULL; i += 2) {
		list_add(list_key_values, key_value_create(argv[i], argv[i + 1]));
	}
}

void key_value_destroy(key_value_t* key_value) {
	free(key_value->key);
	free(key_value->value);
	free(key_value);
}

key_value_t* value_generator(char key[40], size_t size) {
	char* value = string_repeat('X', size);

	key_value_t* key_value = key_value_create(key, value);

	free(value);

	return key_value;
}

size_t required_entries(size_t size) {
	int required_entries = size/entry_size;

	return size % entry_size == 0 ? required_entries : ++required_entries;
}

void print_key_value(key_value_t* key_value) {
	printf("Clave: %s\t Tamanio: %d\t Valor: %s\n", key_value->key, key_value->size, key_value->value);
}

void print_entry(char* key, entry_t* entry) {
	size_t entries_required = required_entries(entry->size);
	char* entrada_text = string_duplicate("entrada");

	if(entries_required > 1) {
		string_append(&entrada_text, "s");
	}

	printf("Clave: %s\t Entrada: %d\t Tamanio: %d\t Ocupa %d %s\n", key, entry->number, entry->size, entries_required, entrada_text);

	free(entrada_text);
}

void print_stored_values(char* key, char* value) {
	printf("Entrada %s:\t", key);

	if(string_length(value) == 0) {
		printf("NULL\n");
	}

	else {
		printf("%s\n", value);
	}
}

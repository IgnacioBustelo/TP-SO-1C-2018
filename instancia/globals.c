#include <commons/string.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"

char* value_to_string(void* value, size_t size) {
	char* string_value = malloc(size + 1);

	memset(string_value, '\0', size + 1);
	memcpy(string_value, value, size);

	return string_value;
}

key_value_t* key_value_create(char* key, char* value) {
	key_value_t* key_value = malloc(sizeof(key_value_t));

	key_value->key = string_substring_until(key, 40);
	key_value->value = string_duplicate(value);
	key_value->size = (size_t) (string_length(value) + 1);

	return key_value;
}

void key_value_destroy(key_value_t* key_value) {
	free(key_value->key);
	free(key_value->value);
	free(key_value);
}

void storage_setup_init(size_t total_entries, size_t entry_size) {
	storage_setup = malloc(sizeof(storage_setup_t));

	storage_setup->total_entries = total_entries;
	storage_setup->entry_size = entry_size;
}

void storage_setup_destroy() {
	free(storage_setup);
}

int get_total_entries(){
	return storage_setup->total_entries;
}

int get_entry_size(){
	return storage_setup->entry_size;
}

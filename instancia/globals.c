#include <commons/string.h>
#include <stdlib.h>

#include "globals.h"

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

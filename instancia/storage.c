#include <commons/string.h>

#include "storage.h"

int storage_init() {
	storage = dictionary_create();

	if(storage == NULL) {
		return STRG_INIT_CREATE_ERROR;
	}

	int i;
	for(i = 0; i < total_entries; i++) {
		char* key = string_itoa(i);

		dictionary_put(storage, key, string_new());

		free(key);
	}

	return STRG_INIT_SUCCESS;
}

static void storage_set_atomic(int next_entry, char* new_value) {
	char* key = string_itoa(next_entry);

	dictionary_remove_and_destroy(storage, key, free);

	dictionary_put(storage, key, string_duplicate(new_value));

	free(key);
}

static void storage_set_non_atomic(int next_entry, char* new_value, size_t size) {
	if(size <= entry_size) {
		storage_set_atomic(next_entry, new_value);
	}

	else {
		char* new_value_current = string_substring_until(new_value, entry_size);
		char* new_value_next = string_substring_from(new_value, entry_size);

		storage_set_atomic(next_entry, new_value_current);

		storage_set_non_atomic(++next_entry, new_value_next, size -= entry_size);

		free(new_value_current);
		free(new_value_next);
	}
}

int storage_set(key_value_t* key_value) {
	int next_entry = storage_next_entry(key_value->size);

	storage_set_non_atomic(next_entry, key_value->value, key_value->size);

	return next_entry;
}

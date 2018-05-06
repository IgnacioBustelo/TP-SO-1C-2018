#include <commons/string.h>
#include <stdlib.h>

#include "globals.h"
#include "storage.h"

void storage_init() {
	storage = dictionary_create();

	int i;
	for(i = 0; i < storage_setup.total_entries; i++) {
		char* key = string_itoa(i);

		dictionary_put(storage, key, string_new());

		free(key);
	}
}

static void storage_set_atomic(int next_entry, char* new_value) {
	char* key = string_itoa(next_entry);

	dictionary_remove_and_destroy(storage, key, free);

	dictionary_put(storage, key, string_duplicate(new_value));

	free(key);
}

static void storage_set_non_atomic(int next_entry, char* new_value, size_t size) {
	if(size <= storage_setup.entry_size) {
		storage_set_atomic(next_entry, new_value);
	}

	else {
		char* new_value_current = string_substring_until(new_value, storage_setup.entry_size);
		char* new_value_next = string_substring_from(new_value, storage_setup.entry_size);

		storage_set_atomic(next_entry, new_value_current);

		storage_set_non_atomic(++next_entry, new_value_next, size -= storage_setup.entry_size);

		free(new_value_current);
		free(new_value_next);
	}
}

void storage_set(int next_entry, key_value_t* key_value) {
	storage_set_non_atomic(next_entry, key_value->value, key_value->size);
}

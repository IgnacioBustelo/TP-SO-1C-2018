#include <commons/string.h>
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

int get_total_entries()
{
	return storage_setup.total_entries;
}

int get_entry_size(){
	return storage_setup.entry_size;
}

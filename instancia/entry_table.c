#include <stdlib.h>

#include "entry_table.h"
#include "storage.h"

int insert_entry(key_value_t* key_value) {
	if(dictionary_has_key(entry_table, key_value->key)) {
		return update_entry(key_value);
	}

	else {
		entry_t* entry = malloc(sizeof(entry_t));

		entry->number = set_value(key_value);
		entry->size = key_value->size;

		dictionary_put(entry_table, key_value->key, (void*) entry);

		return ET_INSERT_SUCCESS;
	}
}

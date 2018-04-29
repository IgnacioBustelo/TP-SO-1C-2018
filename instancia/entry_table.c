#include <stdlib.h>

#include "entry_table.h"
#include "instancia.h"
#include "storage.h"

int entry_table_insert(key_value_t* key_value) {
	if(dictionary_has_key(entry_table, key_value->key)) {
		return entry_table_update(key_value);
	}

	else {
		entry_t* entry = malloc(sizeof(entry_t));

		entry->number = storage_set(key_value);
		entry->size = key_value->size;

		dictionary_put(entry_table, key_value->key, (void*) entry);

		return ET_INSERT_SUCCESS;
	}
}

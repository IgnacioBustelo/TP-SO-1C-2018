#include <stdlib.h>

#include "entry_table.h"
#include "instancia.h"
#include "storage.h"

int entry_table_init() {
	entry_table = dictionary_create();

	if (entry_table != NULL) {
		return ET_INIT_SUCCESS;
	}

	else {
		return ET_INIT_ERROR;
	}
}

int entry_table_insert(key_value_t* key_value) {
	entry_t* entry = malloc(sizeof(entry_t));

	entry->number = storage_set(key_value);
	entry->size = key_value->size;

	dictionary_put(entry_table, key_value->key, (void*) entry);

	return ET_INSERT_SUCCESS;
}


int entry_table_update(key_value_t* key_value){
	if(dictionary_has_key(entry_table,key_value->key))
			{
				entry_t* entry = malloc(sizeof(entry_t));

				entry_t* entry_old = dictionary_get(entry_table,key_value);

				entry->number=entry_old->number;
				entry->size = key_value->size;

				dictionary_remove(entry_table,key_value->key);

				dictionary_put(entry_table, key_value->key, (void*) entry);

				storage_update(key_value);

				return ET_UPDATE_SUCCESS;
			}
	return ET_UPDATE_ERROR;
}

int entry_table_delete(char* key){
	return 1;
}

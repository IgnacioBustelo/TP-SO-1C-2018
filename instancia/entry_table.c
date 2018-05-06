#include <stdlib.h>

#include "entry_table.h"
#include "globals.h"

void entry_table_init() {
	entry_table = dictionary_create();
}

void entry_table_insert(int next_entry, key_value_t* key_value) {
	entry_t* entry = malloc(sizeof(entry_t));

	entry->number = next_entry;
	entry->size = key_value->size;

	dictionary_put(entry_table, key_value->key, (void*) entry);
}


void entry_table_update(int next_entry, key_value_t* key_value){
	if(dictionary_has_key(entry_table,key_value->key))
			{
				entry_t* entry = malloc(sizeof(entry_t));

				entry_t* entry_old = dictionary_get(entry_table, key_value->key);

				entry->number=entry_old->number;
				entry->size = key_value->size;

				dictionary_remove(entry_table,key_value->key);

				dictionary_put(entry_table, key_value->key, (void*) entry);

				// storage_update(key_value); Perdón, pero esto ya no es necesario.
			}
}

void entry_table_delete(char* key){

}

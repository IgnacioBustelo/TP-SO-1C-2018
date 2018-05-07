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


void entry_table_update( key_value_t* key_value){
	if(dictionary_has_key(entry_table,key_value->key))
			{
				entry_t* entry = malloc(sizeof(entry_t));

				entry_t* entry_old = dictionary_get(entry_table, key_value->key);

				entry->number=entry_old->number;
				entry->size = key_value->size;

				dictionary_remove(entry_table,key_value->key);

				dictionary_put(entry_table, key_value->key, (void*) entry);
			}
}

int entry_table_next_entry(key_value_t* key_value) {
	if (entry_table_entries_fit(key_value->size))
		return dictionary_size(entry_table);
return ENTRY_LIMIT_ERROR;
}

static int entries_needed(size_t size){

	int total=0;

	while (size - storage_setup.entry_size > 0)
		{
			total++;
		}
	return total++;
}

static bool entry_table_entries_fit(int entris_number){

	return (dictionary_size(entry_table)+entris_number)>=storage_setup.total_entries;
}



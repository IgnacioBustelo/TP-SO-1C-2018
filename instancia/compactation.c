#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>

#include "../libs/messenger.h"

#include "tests/utils.h"

#include "compactation.h"
#include "entry_table.h"
#include "storage.h"
#include "algorithms.h"

int compactation_compact(){
	for (int i=0;i<list_size(entry_table);i++)
	{
		char*key;
		int size;
		int next_entry;
		key_value_t* key_value;
		entry_t * entry;

		entry=((entry_t *) list_get(entry_table,i));
		key =string_duplicate(entry->key);
		size = entry->size;
		char* value = storage_retrieve_string(entry->number, size);

		key_value = key_value_generator(key,size);

		entry_table_delete(key_value);
		entry_table_status_delete_kv(key_value);

		next_entry = entry_table_next_entry(key_value);

		entry_table_insert(next_entry,key_value);
		entry_table_status_add_kv(key_value,next_entry);

		storage_set(next_entry, value, size);

		free(value);
		free(key);
		key_value_destroy(key_value);
	}

	return 1;
}

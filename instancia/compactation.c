<<<<<<< HEAD
#include "compactation.h"
#include "storage.h"
#include "globals.h"
=======
#include <commons/collections/list.h>
#include <stdlib.h>
>>>>>>> 0b5524018d80c442b0acd2ab3adf748e5f5b4494

#include "tests/utils.h"

#include "compactation.h"
#include "entry_table.h"

int compactation_compact(){
	t_list * key_value_list = list_create();

	char*key;
	int size;

	int next_entry;

	key_value_t* key_value;
	entry_t * entry;
	for (int i=0;i<list_size(entry_table);i++)
	{
		entry=((entry_t *) list_get(entry_table,i));
		key =entry->key;
		size = entry->size;
		void* void_value = storage_retrieve(entry->number, size);
		char* value = messenger_bytes_to_string(void_value, size);

		key_value = key_value_generator(key,size);

		//list_add(key_value_list,key_value);

		entry_table_delete(key_value);

		next_entry = entry_table_next_entry(key_value);

		entry_table_insert(next_entry,key_value);

		storage_set(next_entry, value, size);

		free(void_value);
		free(value);

	}

//	for (int i=0;i<list_size(key_value_list);i++)
//		{
//			key_value = list_get(key_value_list,i);
//			next_entry = entry_table_next_entry(key_value);
//			entry_table_insert(next_entry,key_value);
//			//TODO: Santi, manejar aca agregar en tu storage_Table las entradas.
//		}
	free(key);
	key_value_destroy(key_value);
	list_clean_and_destroy_elements(key_value_list, free);
	list_destroy(key_value_list);

	return 1;
}

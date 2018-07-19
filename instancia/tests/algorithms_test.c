#include <stdlib.h>
#include <stdio.h>

#include "../entry_table.h"
#include "../globals.h"
#include "./utils.h"
#include "../algorithms.h"

int main(void)
{
	algorithm_circular_set_pointer(1);
	storage_setup_init(3,20);
	entry_table_init();
	entry_table_status_init();

	entry_t* entry;

	key_value_t* key_value=key_value_generator("A",20);
	int next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);

	key_value=key_value_generator("B",20);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);


	 key_value=key_value_generator("X",19);
	 next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);

//	key_value=key_value_generator("Y",22);
//	next_entry = entry_table_next_entry(key_value);
//	entry_table_insert(next_entry,key_value);
//	entry_table_status_add_kv(key_value,next_entry);




//	printf("\n");
//	entry_table_print_table(); //X,Y,Z
//
//	printf("\n\n");
//		entry_table_status_print_table(entry_table_status_global);


//	t_list* entry_table_status = original_entry_table_migration_to_complete_one();
//
//	entry_table_status_print_table(entry_table_status);

	key_value=key_value_generator("LRU_A",20);

	t_list* replaced_key = list_create();
	printf("\n\n");
	printf("Intento Aplicar algoritmo circular \n\n");
	printf("\n\n");
//	algorithm_circular(entry_table_status_global,key_value,replaced_key);
	if (algorithm_bsu(entry_table_status_global,key_value,replaced_key))
	{
	for(int i=0;i<list_size(replaced_key);i++)
	{
		entry=entry_table_get_entry(list_get(replaced_key,i));
		key_value= key_value_generator(entry->key,entry->size);
		entry_table_delete(key_value);
		entry_table_status_delete_kv(key_value);
	}
	key_value=key_value_generator("LRU_A",20);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);
	printf("Aplique algoritmo circular \n\n");
	}

	list_destroy_and_destroy_elements(replaced_key,free);
		 replaced_key = list_create();
			key_value=key_value_generator("LRU_B",20);
		printf("\n\n");
		printf("Intento Aplicar algoritmo circular \n\n");
		printf("\n\n");
	//	algorithm_circular(entry_table_status_global,key_value,replaced_key);
		if (algorithm_bsu(entry_table_status_global,key_value,replaced_key))
		{
		for(int i=0;i<list_size(replaced_key);i++)
		{
			entry=entry_table_get_entry(list_get(replaced_key,i));
			key_value= key_value_generator(entry->key,entry->size);
			entry_table_delete(key_value);
			entry_table_status_delete_kv(key_value);
		}
		key_value=key_value_generator("LRU_B",20);
		next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);
		entry_table_status_add_kv(key_value,next_entry);
		printf("Aplique algoritmo circular \n\n");
		}

//	printf("\n\n");
//	entry_table_print_table();
//	printf("\n Entries Left = %d",entries_left);
//		printf("\n\n");
//		entry_table_status_print_table(entry_table_status_global);
//
//	list_destroy_and_destroy_elements(replaced_key,free);
//	key_value=key_value_generator("LRU_B",10);
//	replaced_key = list_create();
//	if (algorithm_circular(entry_table_status_global,key_value,replaced_key))
//	{
//		for(int i=0;i<list_size(replaced_key);i++)
//		{
//			entry=entry_table_get_entry(list_get(replaced_key,i));
//			key_value= key_value_generator(entry->key,entry->size);
//			entry_table_delete(key_value);
//			entry_table_status_delete_kv(key_value);
//		}
//	key_value=key_value_generator("LRU_B",10);
//	next_entry = entry_table_next_entry(key_value);
//	entry_table_insert(next_entry,key_value);
//	entry_table_status_add_kv(key_value,next_entry);
//	printf("Aplique algoritmo circular \n\n");
//		}
//
//	printf("\n\n");
//		entry_table_print_table();
//		printf("\n Entries Left = %d",entries_left);
//		printf("\n\n");
//		entry_table_status_print_table(entry_table_status_global);
//
//	list_destroy_and_destroy_elements(replaced_key,free);
//	key_value=key_value_generator("BSU",10);
//	replaced_key = list_create();
//		if (algorithm_bsu(entry_table,key_value,replaced_key))
//		{
//			for(int i=0;i<list_size(replaced_key);i++)
//			{
//				key_value= key_value_generator(list_get(replaced_key,i),0);
//				entry_table_delete(key_value);
//				entry_table_status_delete_kv(key_value);
//			}
//		key_value=key_value_generator("BSU",10);
//		next_entry = entry_table_next_entry(key_value);
//		entry_table_insert(next_entry,key_value);
//		entry_table_status_add_kv(key_value,next_entry);
//		printf("Aplique algoritmo circular \n\n");
//			}
////

	printf("\n\n");
	entry_table_print_table();
	printf("\n Entries Left = %d",entries_left);
	printf("\n\n");
	entry_table_status_print_table(entry_table_status_global);

}

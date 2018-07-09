#include <stdlib.h>
#include <stdio.h>

#include "../entry_table.h"
#include "../globals.h"
#include "./utils.h"
#include "../algorithms.h"

int main(void)
{
	algorithm_circular_set_pointer(0);
	storage_setup_init(5,20);
	entry_table_init();
	entry_table_status_init();

	key_value_t* key_value=key_value_generator("A",15);
	int next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);

	key_value=key_value_generator("B",15);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);


	 key_value=key_value_generator("X",15);
	 next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);

	key_value=key_value_generator("Y",22);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);




	printf("\n");
	entry_table_print_table(); //X,Y,Z

	printf("\n\n");
		entry_table_status_print_table(entry_table_status_global);


//	t_list* entry_table_status = original_entry_table_migration_to_complete_one();
//
//	entry_table_status_print_table(entry_table_status);

	key_value=key_value_generator("FF",20);

	t_list* replaced_key = list_create();
	printf("\n\n");
	printf("Intento Aplicar algoritmo circular \n\n");
	printf("\n\n");

	if (algorithm_lru(entry_table,key_value,replaced_key))
	{
	for(int i=0;i<list_size(replaced_key);i++)
	{
		key_value= key_value_generator(list_get(replaced_key,i),0);
		entry_table_delete(key_value);
		entry_table_status_delete_kv(key_value);
	}
	key_value=key_value_generator("FF",20);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);
	printf("Aplique algoritmo circular \n\n");
	}



	list_destroy(replaced_key);
	key_value=key_value_generator("AZ",10);
//	replaced_key = list_create();
	if (algorithm_lru(entry_table,key_value,replaced_key))
	{
		for(int i=0;i<list_size(replaced_key);i++)
		{
			key_value= key_value_generator(list_get(replaced_key,i),0);
			entry_table_delete(key_value);
			entry_table_status_delete_kv(key_value);
		}
	key_value=key_value_generator("AZ",10);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);
	entry_table_status_add_kv(key_value,next_entry);
	printf("Aplique algoritmo circular \n\n");
		}



	entry_table_print_table();
	printf("\n\n");
	entry_table_status_print_table(entry_table_status_global);

}

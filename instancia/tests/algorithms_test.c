#include <stdlib.h>
#include <stdio.h>

#include "../entry_table.h"
#include "../globals.h"
#include "./utils.h"
#include "../algorithms.h"

int main(void)
{

	storage_setup_init(10,20);
	entry_table_init();
	key_value_t* key_value=key_value_generator("X",30);
	int next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);

	key_value=key_value_generator("Y",30);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);

	key_value=key_value_generator("Z",15);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);

	printf("\n");
	entry_table_print_table(); //X,Y,Z

	printf("Entries left: %d\n",entries_left);

	key_value=key_value_generator("X",30);
	entry_table_delete(key_value);

	printf("\n");
	entry_table_print_table(); //X,Y

	printf("Entries left: %d\n",entries_left);

	key_value=key_value_generator("A",15);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);

	printf("\n");
	entry_table_print_table(); //X,Y

	printf("Entries left: %d\n",entries_left);

//	t_list* entry_table_status = original_entry_table_migration_to_complete_one();
//
//	entry_table_status_print_table(entry_table_status);

	key_value=key_value_generator("T",30);

	algorithm_circular_set_pointer(0);

	t_list* replaced_key = list_create();

	algorithm_circular(entry_table,key_value,replaced_key);

	printf("Aplico algoritmo circular Entries left: %d\n",entries_left);

	printf("\n\n");

	for(int i=0;i<list_size(replaced_key);i++)
	{
		entry_table_delete(key_value_generator("A",15));
	}

	entry_table_print_table();

//	key_value=key_value_generator("T",30);
//
//	next_entry = entry_table_next_entry(key_value);
//	entry_table_insert(next_entry,key_value);
//
//
//
//	entry_table_print_table();


	printf("Deberia guardar el nuevo kv Entries left: %d\n",entries_left);

}

#include <stdlib.h>
#include <stdio.h>

#include "../entry_table.h"
#include "../globals.h"
#include "./utils.h"
#include "../algorithms.h"

int main(void)
{

	storage_setup_init(5,20);
	entry_table_init();

	key_value_t* key_value=key_value_generator("A",15);
	int next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);

	key_value=key_value_generator("B",15);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);


	 key_value=key_value_generator("X",15);
	 next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);

	key_value=key_value_generator("Y",15);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);

	key_value=key_value_generator("Z",15);
	next_entry = entry_table_next_entry(key_value);
	entry_table_insert(next_entry,key_value);



	printf("\n");
	entry_table_print_table(); //X,Y,Z


//	t_list* entry_table_status = original_entry_table_migration_to_complete_one();
//
//	entry_table_status_print_table(entry_table_status);

	key_value=key_value_generator("FF",99);

	algorithm_circular_set_pointer(0);

	t_list* replaced_key = list_create();



	printf("\n\n");

	printf("Intento Aplicar algoritmo circular \n\n");

	printf("\n\n");

	if (algorithm_circular(entry_table,key_value,replaced_key))
	{
	for(int i=0;i<list_size(replaced_key);i++)
	{
		key_value= key_value_generator(list_get(replaced_key,i),0);
		entry_table_delete(key_value);

	}
	printf("Aplique algoritmo circular \n\n");
	}


//
//	key_value=key_value_generator("FF",30);
//	next_entry = entry_table_next_entry(key_value);
//	entry_table_insert(next_entry,key_value);

		entry_table_print_table();


}

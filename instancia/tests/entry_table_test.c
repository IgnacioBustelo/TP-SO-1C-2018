#include <stdlib.h>
#include <stdio.h>

#include "../entry_table.h"
#include "../globals.h"
#include "./utils.h"

int main()
{

		storage_setup_init(20,20);
		entry_table_init();
		key_value_t* key_value=key_value_generator("X",15);
		int next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);

		key_value=key_value_generator("Y",30);
		next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);

		key_value=key_value_generator("Z",15);
		next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);

		key_value=key_value_generator("Y",30);
		entry_table_delete(key_value);
//		printf("\n");
//		entry_table_print_table(); //X,Y,Z
//
//		key_value=key_value_generator("X",30);
//		entry_table_delete(key_value);
//
//		printf("\n");
//		entry_table_print_table(); //X,Y

		key_value=key_value_generator("X",50);
		next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);


		printf("\n %d \n",entries_left);
		entry_table_print_table(); //X,Y
}



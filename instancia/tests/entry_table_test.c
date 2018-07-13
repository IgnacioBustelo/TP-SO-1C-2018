#include <stdlib.h>
#include <stdio.h>

#include "../entry_table.h"
#include "../globals.h"
#include "./utils.h"

int main()
{

		storage_setup_init(5,10);
		entry_table_init();
		entry_table_status_init();
//		//Caso a
//		key_value_t* key_value=key_value_generator("X",9);
//		int next_entry = entry_table_next_entry(key_value);
//		entry_table_insert(next_entry,key_value);
//
//		key_value=key_value_generator("Y",29);
//		next_entry = entry_table_next_entry(key_value);
//		entry_table_insert(next_entry,key_value);
//
//		key_value=key_value_generator("Z",9);
//		next_entry = entry_table_next_entry(key_value);
//		entry_table_insert(next_entry,key_value);
//
//		key_value=key_value_generator("Y",29);
//		entry_table_delete(key_value);
//
//
//		key_value=key_value_generator("X",39);
//		next_entry = entry_table_next_entry(key_value);
//		entry_table_insert(next_entry,key_value);
//
//
//		printf("\n %d \n",entries_left);
//		entry_table_print_table(); //X,Y
//
//		entry_table_destroy();

//		//Caso b
//		key_value_t* key_value=key_value_generator("X",10);
//				int next_entry = entry_table_next_entry(key_value);
//				entry_table_insert(next_entry,key_value);
//
//				key_value=key_value_generator("Y",30);
//				next_entry = entry_table_next_entry(key_value);
//				entry_table_insert(next_entry,key_value);
//
//				key_value=key_value_generator("Z",10);
//				next_entry = entry_table_next_entry(key_value);
//				entry_table_insert(next_entry,key_value);
//////
//				key_value=key_value_generator("Y",30);
//				entry_table_delete(key_value);
//
//
//				key_value=key_value_generator("X",40);
//				next_entry = entry_table_next_entry(key_value);
//				entry_table_insert(next_entry,key_value);
//
//
//				printf("\n %d \n",entries_left);
//				entry_table_print_table(); //X,Y
//
//				entry_table_destroy();

//		//Caso c
		key_value_t* key_value=key_value_generator("X",10);
				int next_entry = entry_table_next_entry(key_value);
				entry_table_insert(next_entry,key_value);
				entry_table_status_add_kv(key_value,next_entry);


				key_value=key_value_generator("Y",10);
				next_entry = entry_table_next_entry(key_value);
				entry_table_insert(next_entry,key_value);
				entry_table_status_add_kv(key_value,next_entry);


				key_value=key_value_generator("Z",30);
				next_entry = entry_table_next_entry(key_value);
				entry_table_insert(next_entry,key_value);
				entry_table_status_add_kv(key_value,next_entry);


				key_value=key_value_generator("Z",30);
				entry_table_delete(key_value);
				entry_table_status_delete_kv(key_value);

				key_value=key_value_generator("X",30);
				next_entry = entry_table_next_entry(key_value);
				entry_table_insert(next_entry,key_value);
				entry_table_status_add_kv(key_value,next_entry);

				key_value=key_value_generator("A",10);
				next_entry = entry_table_next_entry(key_value);
				entry_table_insert(next_entry,key_value);
				entry_table_status_add_kv(key_value,next_entry);


				printf("\n %d \n",entries_left);
				entry_table_print_table();//X,Y
				printf("\n  \n");
				entry_table_status_print_table();
				entry_table_destroy();



}



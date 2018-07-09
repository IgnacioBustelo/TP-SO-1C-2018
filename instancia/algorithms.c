#include <stdio.h>

#include "algorithms.h"
#include "globals.h"

void algorithms_exec(char algorithm_id, t_list* entry_table, key_value_t* key_value, t_list* replaced_keys) {
	switch (algorithm_id) {
		case 'C':	algorithm_circular(entry_table, key_value, replaced_keys);	break;

		case 'L':	/*TODO: Desarrollar algoritmo Least Recently Used*/			break;

		case 'B':	/*TODO: Desarrollar algoritmo Biggest Space Used*/			break;

		default:	/*TODO: Generar error*/										break;
	}
}

void algorithm_circular_set_pointer(int index){
	algorithm_circular_pointer=index;
}

int algorithm_circular(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys){ //TODO: Creo que hay que reimplementar con la logica de lru
	if(!entry_table_have_entries(key_value) && new_value_fits(key_value))
	{

		int continous_atomic_and_free_entries=0;
		int first_entry_of_continous_atomic_and_free_entries=-1;
		t_list* entry_table_status = original_entry_table_migration_to_entry_table_status();
		status_t* status;
		if (algorithm_circular_pointer>=list_size(entry_table_status))
		{

			algorithm_circular_pointer=0;
		}
		while(algorithm_circular_pointer<list_size(entry_table_status) && continous_atomic_and_free_entries!=entry_table_entries_needed(key_value))
		{

			status = (status_t*)list_get(entry_table_status,algorithm_circular_pointer);
			if ((status->status==ATOMIC || status->status==FREE) && first_entry_of_continous_atomic_and_free_entries == -1)
			{
				first_entry_of_continous_atomic_and_free_entries = algorithm_circular_pointer;
				continous_atomic_and_free_entries++;
			}
			else if ((status->status==ATOMIC || status->status==FREE) && first_entry_of_continous_atomic_and_free_entries != -1)
			{
				continous_atomic_and_free_entries++;

			}
			else
			{
				continous_atomic_and_free_entries=0;
				first_entry_of_continous_atomic_and_free_entries=-1;
			}
			algorithm_circular_pointer++;
		}

		if (continous_atomic_and_free_entries==entry_table_entries_needed(key_value))
		{


			while (continous_atomic_and_free_entries>0)
			{
				status = (status_t*)list_get(entry_table_status,first_entry_of_continous_atomic_and_free_entries);
				if (status->status==ATOMIC)
				{
					list_add(replaced_keys,status->key);
				}
				first_entry_of_continous_atomic_and_free_entries++;
				continous_atomic_and_free_entries--;
			}

		return 1;
		}
	}

	return 0;
}


bool smallest_reference(void * a, void *b){
	status_t * e1 = (status_t*)a;
	status_t * e2 = (status_t*)b;
	return e1->last_referenced<e2->last_referenced?false:true;
}

int algorithm_lru(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys){
	if(!entry_table_have_entries(key_value) && new_value_fits(key_value))
	{
		t_list* entry_table_status = entry_table_status_global;
		status_t* status;


		t_list * copy_entry_table_status = list_create();

		list_add_all(copy_entry_table_status,entry_table_status);
		list_sort(copy_entry_table_status,smallest_reference);

		int entries_neeeded = entry_table_entries_needed(key_value) - entries_left;

		int i=0;
		while(i<list_size(copy_entry_table_status) && entries_neeeded)
		{
			status = (status_t*)list_get(copy_entry_table_status,i);
			if (status->status==ATOMIC)
			{
				list_add(replaced_keys,status->key);
				entries_neeeded--;
			}
			i++;
		}
	return 1;
	}

	else {
		return -666; // TODO: Santi - Agregue esto porque no compila nada... Segui tu logica para el else de tu if.
	}
}

bool new_value_fits(key_value_t* key_value)
{
	return entries_left+entry_table_atomic_entries_count()>=entry_table_entries_needed(key_value);
}

t_list* original_entry_table_migration_to_entry_table_status()
{
	t_list * entry_table_status = list_create();

	entry_t * entry=NULL;

	int entry_entries=0;

	status_t * status = malloc(sizeof(status_t));
	status->last_referenced=0;
	status->status=FREE;

	int i=0;
	while(i<get_total_entries())
	{
	entry=entry_table_get_entry_by_entry_number(i);
	 if((entry)!=NULL){
		 if(entry_table_is_entry_atomic(entry))
		 {
			 list_add(entry_table_status,convert_entry_t_to_status_t(entry));
			 i++;
		 }
		 else
		 {
			 int entries=(entry->size/get_entry_size());
			 entry_entries = entry->size%get_entry_size()==0?entries:entries+1;
//			 entry_entries = ((entry->size)/get_entry_size())+1;
			 while (entry_entries>0)
			 {

				 list_add(entry_table_status,convert_entry_t_to_status_t(entry));
				 entry_entries-=1;
				 i++;
			 }
			 entry_entries=0;
			 entries=0;
		 }
	 }
	 else
	 {
		 list_add(entry_table_status,status);
		 i++;
	 }
	}

return entry_table_status;

}

void entry_table_status_init(){
	entry_table_status_global = original_entry_table_migration_to_entry_table_status();
}

void entry_table_status_add_kv(key_value_t* key_value,int number){ // Reeveer logica de como agrego a la tabla las cosas!

	entry_table_status_last_referenced_add_all();
	entry_t * entry=convert_key_value_t_to_entry_t(key_value);
	int entry_entries=0;

	status_t * status = malloc(sizeof(status_t));
	status->status=FREE;
	status->last_referenced=0;

		 if(entry_table_is_entry_atomic(entry))
		 {
			 list_replace(entry_table_status_global,number,convert_entry_t_to_status_t(entry));
		 }
		 else
		 {
			 int entries=(entry->size/get_entry_size());
			 entry_entries = entry->size%get_entry_size()==0?entries:entries+1;
			 int number_copy=number;
			 while (entry_entries>=0)
			 {
				 list_replace(entry_table_status_global,number_copy,convert_entry_t_to_status_t(entry));
				 entry_entries-=1;
				 number_copy++;
			 }
		 }

}

void entry_table_status_delete_kv(key_value_t* key_value){
	int entry_entries=0;

	int i=0;
	int deleted=0;
		while(i<get_total_entries() && deleted!=entry_table_entries_needed(key_value))
		{
			status_t* entry_status = (status_t*)(list_get(entry_table_status_global,i));
			if(!strcasecmp(entry_status->key,key_value->key))
			{
				entry_status->last_referenced=0;
				free(entry_status->key);
				entry_status->key=strdup("NULL");
				entry_status->status=FREE;
				deleted++;
			}
			i++;
		}

}

void entry_table_status_last_referenced_add_all(){
	int i=0;
	status_t* status;
	while(i<get_total_entries())
			{
				status=list_get(entry_table_status_global,i);
				if(status->status!=FREE)
					status->last_referenced++;
				i++;
			}
}

status_t * convert_entry_t_to_status_t(entry_t* entry){
	status_t * status = malloc(sizeof(status_t));
	status->key = strdup(entry->key);
	status->last_referenced=0;
	if (entry_table_is_entry_atomic(entry))
	status->status=ATOMIC;
	else
	status->status=NON_ATOMIC;
	return status;
}

void entry_table_status_print_table(t_list* entry_table_status){

	for (int i=0; i<list_size(entry_table_status);i++)
		{
		status_t * status=(status_t *) list_get(entry_table_status,i);
		printf("Indice %d con estado %d, KEY %s, Referenciado hace:%d \n",i,status->status,status->key,status->last_referenced);
		}
}

#include <commons/string.h>
#include <stdio.h>

#include "../libs/messenger.h"

#include "algorithms.h"
#include "globals.h"

void status_t_destroy(status_t* status) {
	free(status->key);
	free(status);
}

int algorithms_exec(char algorithm_id, t_list* entry_table, key_value_t* key_value, t_list* replaced_keys) {
	switch (algorithm_id) {
		case 'C':	return algorithm_circular(entry_table, key_value, replaced_keys);	break;

		case 'L':	return algorithm_lru(entry_table, key_value, replaced_keys);		break;

		case 'B':	return algorithm_bsu(entry_table, key_value, replaced_keys);		break;

		default:	return -1; /*TODO: Generar error*/									break;
	}
}

void algorithm_circular_set_pointer(int index){
	algorithm_circular_pointer=index;
}

int algorithm_circular(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys){ //TODO: Creo que hay que reimplementar con la logica de lru
if (!entry_table_have_entries(key_value) && new_value_fits(key_value))
	{

//		int continous_atomic_and_free_entries=0;
//		int first_entry_of_continous_atomic_and_free_entries=-1;
//		t_list* entry_table_status = original_entry_table_migration_to_entry_table_status();
//		status_t* status;
//		if (algorithm_circular_pointer>=list_size(entry_table_status))
//		{
//
//			algorithm_circular_pointer=0;
//		}
//		while(algorithm_circular_pointer<list_size(entry_table_status) && continous_atomic_and_free_entries!=entry_table_entries_needed(key_value))
//		{
//
//			status = (status_t*)list_get(entry_table_status,algorithm_circular_pointer);
//			if ((status->status==ATOMIC || status->status==FREE) && first_entry_of_continous_atomic_and_free_entries == -1)
//			{
//				first_entry_of_continous_atomic_and_free_entries = algorithm_circular_pointer;
//				continous_atomic_and_free_entries++;
//			}
//			else if ((status->status==ATOMIC || status->status==FREE) && first_entry_of_continous_atomic_and_free_entries != -1)
//			{
//				continous_atomic_and_free_entries++;
//
//			}
//			else
//			{
//				continous_atomic_and_free_entries=0;
//				first_entry_of_continous_atomic_and_free_entries=-1;
//			}
//			algorithm_circular_pointer++;
//		}
//
//		if (continous_atomic_and_free_entries==entry_table_entries_needed(key_value))
//		{
//
//
//			while (continous_atomic_and_free_entries>0)
//			{
//				status = (status_t*)list_get(entry_table_status,first_entry_of_continous_atomic_and_free_entries);
//				if (status->status==ATOMIC)
//				{
//					list_add(replaced_keys,strdup(status->key));
//					status->status=USED;
//
//				}
//				first_entry_of_continous_atomic_and_free_entries++;
//				continous_atomic_and_free_entries--;
//			}
//
//		list_destroy_and_destroy_elements(entry_table_status, (void*) status_t_destroy);
//		return 1;
//		}
//	}

		t_list* entry_table_status = entry_table;
		status_t* status;
		int algorithm_circular_pointer_copy = algorithm_circular_pointer;
		int entries_neeeded = entry_table_entries_needed(key_value) - entries_left;
		if (algorithm_circular_pointer>=list_size(entry_table_status) || algorithm_circular_pointer==0)
		{
			algorithm_circular_pointer=0;
				while(algorithm_circular_pointer<list_size(entry_table_status) && entries_neeeded)
				{
					status = (status_t*)list_get(entry_table_status,algorithm_circular_pointer);
					if (status->status==ATOMIC)
					{
						list_add(replaced_keys,strdup(status->key));
						status->status=USED;
						entries_neeeded--;
					}
					algorithm_circular_pointer++;
				}
		}
		else
		{
			while(algorithm_circular_pointer<list_size(entry_table_status) && entries_neeeded)
			{
				status = (status_t*)list_get(entry_table_status,algorithm_circular_pointer);
				if (status->status==ATOMIC)
				{
					list_add(replaced_keys,strdup(status->key));
					status->status=USED;
					entries_neeeded--;
				}
				algorithm_circular_pointer++;
			}
			int i=0;
			while(i<algorithm_circular_pointer_copy && entries_neeeded)
			{
				status = (status_t*)list_get(entry_table_status,i);
				if (status->status==ATOMIC)
				{
					list_add(replaced_keys,strdup(status->key));
					status->status=USED;
					entries_neeeded--;
				}
				i++;
			}
		}
			//list_destroy(entry_table_status);
			return 1;
}else{
	return -1;
}
}


bool smallest_reference(void * a, void *b){
	status_t * e1 = (status_t*)a;
	status_t * e2 = (status_t*)b;
	return e1->last_referenced<e2->last_referenced?false:true;
}

bool biggest_size(void * a, void *b){
	status_t * e1 = (status_t*)a;
	status_t * e2 = (status_t*)b;
	return e1->space_used<e2->space_used?false:true;
}

int algorithm_lru(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys){
	if(!entry_table_have_entries(key_value) && new_value_fits(key_value))
	{
		t_list* entry_table_status = entry_table;
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
				list_add(replaced_keys,strdup(status->key));
				status->status=USED;
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

int algorithm_bsu(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys){
	if(!entry_table_have_entries(key_value) && new_value_fits(key_value))
	{
		t_list* entry_table_status = entry_table;

//		t_list * copy_entry_table_status = list_create();
//		list_add_all(copy_entry_table_status,entry_table_status);
//		list_sort(copy_entry_table_status,biggest_size);
		status_t* status;

		int entries_neeeded = entry_table_entries_needed(key_value) - entries_left;

		t_list * status_entries_with_same_size = list_create();
		status_t* status_same_size;
		int i=0;
		while(i<list_size(entry_table_status) && entries_neeeded)
		{
			status = (status_t*)list_get(entry_table_status,i);
			if (status->status==ATOMIC)
			{
				int j=i;
				while(j<list_size(entry_table_status))
				{
					status_same_size = (status_t*)list_get(entry_table_status,j);
					if(status_same_size->space_used==status->space_used)
					{
						list_add(status_entries_with_same_size,status_same_size);
					}
					j++;
				}
				if(list_size(status_entries_with_same_size)>1)
				{
					algorithm_circular(status_entries_with_same_size,key_value,replaced_keys);
				}
				else
				{
					list_add(replaced_keys,strdup(status->key));
					status->status=USED;
					entries_neeeded--;
				}

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

bool new_value_fits_with_replaced(key_value_t* key_value)
{
	return entry_table_diff_entries(key_value)<=entries_left+entry_table_atomic_entries_count();
}

t_list* original_entry_table_migration_to_entry_table_status()
{
	t_list * entry_table_status = list_create();

	entry_t * entry=NULL;

	int entry_entries=0;

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
		status_t * status = malloc(sizeof(status_t));
		status->last_referenced=0;
		status->status=FREE;
		status->space_used=0;
		status->key = string_new();

		list_add(entry_table_status,status);
		i++;
	 }
	}

//free(status);
return entry_table_status;

}

void entry_table_status_init(){
	entry_table_status_global = original_entry_table_migration_to_entry_table_status();
}

void entry_table_status_add_kv(key_value_t* key_value,int number){ // TODO:FIJARSE LOGICA DE COMO MODIFICAR SI YA EXISTE
	entry_table_status_last_referenced_add_all();
	entry_t * entry=convert_key_value_t_to_entry_t(key_value);
	entry->number = number;
	int entry_entries=0;
	/*

	TENES TOTALMENTE AL PEDO ESTO!!!!!!

	status_t * status = malloc(sizeof(status_t));
	status->status=FREE;
	status->last_referenced=0;
	status->space_used=0;*/
if(number>=0)
{
		 if(entry_table_is_entry_atomic(entry))
		 {
			 list_replace_and_destroy_element(entry_table_status_global,number,convert_entry_t_to_status_t(entry), (void*) status_t_destroy);
		 }
		 else
		 {
			 int entries=(entry->size/get_entry_size());
			 entry_entries = entry->size%get_entry_size()==0?entries:entries+1;
			 int number_copy=number;
			 while (entry_entries>0)
			 {
				 list_replace_and_destroy_element(entry_table_status_global,number_copy,convert_entry_t_to_status_t(entry), (void*) status_t_destroy);
				 entry_entries-=1;
				 number_copy++;
			 }
		 }
}
entry_table_key_value_destroy(entry);
//free(status);
}

void entry_table_status_delete_kv(key_value_t* key_value){
	int i=0;
	int deleted=0;
	status_t* entry_status;
		while(i<get_total_entries() && deleted!=entry_table_entries_needed(key_value))
		{
			 entry_status = (status_t*)(list_get(entry_table_status_global,i));
			if(!strcasecmp(entry_status->key,key_value->key))
			{
				entry_status->last_referenced=0;
				free(entry_status->key);
				entry_status->key=strdup("NULL");
				entry_status->status=FREE;
				entry_status->space_used=0;
				deleted++;
			}
			i++;
		}

}

bool entry_table_status_continuous_entries(t_list* replaced_keys)
{
	int i=0;
	char* key;
	int last_index=0;
	while(i<list_size(replaced_keys))
			{
				key = (char*)(list_get(replaced_keys,i));
				if (last_index==0)
				{
					last_index= entry_table_status_find_and_get_index(key);
				}
				else
				{
					if(entry_table_status_find_and_get_index(key)!=last_index+1)
					{
						return 0;
					}
					last_index=entry_table_status_find_and_get_index(key);
				}

			}
	return 1;
}

int entry_table_status_find_and_get_index(char * key){
	int i=0;
	status_t* entry_status;
	while(i<get_total_entries())
				{
					entry_status = (status_t*)(list_get(entry_table_status_global,i));
					if (!strcasecmp(key,entry_status->key))
					{
						return i;
					}
					i++;
				}
	return -1;
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
	status->space_used=-1;
	if (entry_table_is_entry_atomic(entry)){
		status->status=ATOMIC;
		status->space_used=entry->size;
	}
	else{
	status->status=NON_ATOMIC;
	}
	return status;
}

void entry_table_status_print_table(){

	for (int i=0; i<list_size(entry_table_status_global);i++)
		{
		status_t * status=(status_t *) list_get(entry_table_status_global,i);
		messenger_show("INFO","Indice %d con estado %d, KEY %s, Referenciado hace: %d y Ocupa bits: %d",i,status->status,status->key,status->last_referenced,status->space_used);

		}
}

void entry_table_status_destroy() {
	list_destroy_and_destroy_elements(entry_table_status_global, (void*) status_t_destroy);
}

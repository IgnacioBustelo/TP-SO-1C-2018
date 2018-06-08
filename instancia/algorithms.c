#include <stdio.h>
#include "algorithms.h"


void algorithm_circular_init(){
	algorithm_circular_pointer=0;
}

int algorithm_circular(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys){
	if(new_value_fits(key_value))
	{
		int continous_atomic_and_free_entries=0;
		int first_entry_of_continous_atomic_and_free_entries=0;
		if (algorithm_circular_pointer>=list_size(entry_table))
		{
			algorithm_circular_pointer=0;
		}
		while(algorithm_circular_pointer<list_size(entry_table))
		{
			entry_t* entry = (entry_t *)list_get(entry_table,algorithm_circular_pointer);
				if(entry_table_is_entry_atomic(entry))
				{
				}
		}
	}
	return 0;
}

static bool new_value_fits(key_value_t* key_value)
{
	return entries_left+entry_table_atomic_entries_count()>=entry_table_entries_needed(key_value);
}

t_list* original_entry_table_migration_to_complete_one()
{
	t_list * entry_table_status = list_create();

	entry_t * entry=NULL;

	int entry_entries=0;

	status_t * status = malloc(sizeof(status_t));
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

static status_t * convert_entry_t_to_status_t(entry_t* entry){
	status_t * status = malloc(sizeof(status_t));
	status->key = strdup(entry->key);
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
		printf("Indice %d con estado %d y KEY %s \n",i,status->status,status->key);
		}
}

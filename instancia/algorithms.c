#include <stdio.h>
#include "algorithms.h"


void algorithm_circular_set_pointer(int index){
	algorithm_circular_pointer=index;
}

int algorithm_circular(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys){
	if(!entry_table_have_entries(key_value) && new_value_fits(key_value))
	{

		int continous_atomic_and_free_entries=0;
		int first_entry_of_continous_atomic_and_free_entries=-1;
		t_list* entry_table_status = original_entry_table_migration_to_complete_one();
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
		key_value_t * fake_key_value=malloc(sizeof(key_value_t));

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

#include <stdlib.h>
#include <stdio.h>

#include "tests/utils.h"
#include "entry_table.h"
#include "globals.h"


void entry_table_init() {
	entry_table = list_create();
	entries_left = get_total_entries();
}

bool entry_table_insert(int next_entry, key_value_t* key_value)
{
	entry_t * new_entry=convert_key_value_t_to_entry_t(key_value);
	new_entry->number=next_entry;
	if(next_entry>=0)
	{
		if(entry_table_get_entry_by_entry_number(next_entry)==NULL)
		{
		list_add(entry_table,(void *)new_entry);
		entries_left-=entry_table_entries_needed(key_value);
		list_sort(entry_table,ascending);

		}else
		{
		entries_left += entry_table_entries_needed(key_value_generator(entry_table_get_entry(key_value->key)->key,entry_table_get_entry(key_value->key)->size));
		list_replace(entry_table,next_entry,new_entry);
		entries_left -= entry_table_entries_needed(key_value);
		}
		return true;
	}

return false;
}

t_list* entry_table_get_key_list()
{
	t_list* key_values = list_create();
	for(int i=0;i<list_size(entry_table);i++)
	{
		list_add(key_values,list_get(entry_table,i));
	}
	return key_values;
}

bool entry_table_has_key(char* key,bool is_new){
	if(!is_new)
	{
		for (int i=0; i<list_size(entry_table);i++)
		{
			entry_t * entry = (entry_t*) list_get(entry_table,i);
			if (!strcmp(key,entry->key))
				return true;
		}
	}
	return false;
}

void entry_table_key_value_destroy(entry_t * entry)
{
	free(entry->key);
	free(entry);
}

void entry_table_destroy()
{
	list_destroy_and_destroy_elements(entry_table,entry_table_key_value_destroy);
}


bool ascending(void * a, void *b){
	entry_t * e1 = (entry_t*)a;
	entry_t * e2 = (entry_t*)b;
	return e1->number>e2->number?false:true;
}

entry_t* entry_table_get_entry(char* key) {

	for (int i=0; i<list_size(entry_table);i++)
			{
				entry_t * entry = (entry_t*) list_get(entry_table,i);
				if (!strcmp(key,entry->key))
					return entry;
			}
	return NULL;
}


entry_t* entry_table_get_entry_by_entry_number(int number) {

	for (int i=0; i<list_size(entry_table);i++)
			{
				entry_t * entry = (entry_t*) list_get(entry_table,i);
				if (entry->number==number)
					return entry;
			}
	return NULL;
}

int entry_table_next_entry(key_value_t* key_value){

	int in_beetwen_entry_space=0;
	int entries_used=list_size(entry_table);

	int entries_needed = entry_table_entries_needed(key_value);

	entry_t* e1;
	entry_t* e2;

	entry_t * entry_to_be_modified = entry_table_get_entry(key_value->key);
	 if (entry_to_be_modified!=NULL )
	    {
	    	int extra_entries_needed=0;
			if(key_value->size<=(entry_to_be_modified->size))
			{
				return entry_to_be_modified->number;
			}
			else
			{

				key_value_t* existing_kv = key_value_generator(entry_to_be_modified->key,entry_to_be_modified->size);
				extra_entries_needed = entries_needed - entry_table_entries_needed(existing_kv);
				key_value_t * key_value_replaced = key_value_generator("X",extra_entries_needed*get_entry_size());
				if(entry_table_have_entries(key_value_replaced))
				{

					int i=0;
					int number = entry_table_get_entry(key_value->key)->number+entry_table_entries_needed(existing_kv);
					printf("\n %d \n", number);
					while (entry_table_get_entry_by_entry_number(number+i)==NULL && i<extra_entries_needed)
					{
						i++;
					}
					if (i==extra_entries_needed)
					{
						return entry_table_get_entry(key_value->key)->number;
					}
				}
			}
	    }
	else if(entry_table_have_entries(key_value))
    {
    	if (entry_table!=NULL )
    	{
    		if(entries_used==0)
    			return 0;
    		else
    		{
    			e1 = (entry_t*) list_get(entry_table,0);

    			if (e1->number>0)
				{
					if ((e1->number)>=entries_needed)
					{
						return 0;
					}
				}
				for (int i=0;i+1<entries_used;i++)
				{

					e1 = (entry_t*) list_get(entry_table,i);
					e2 = (entry_t*) list_get(entry_table,i+1);

					in_beetwen_entry_space = (e2->number)-(e1->number+(e1->size/get_entry_size()+1));

					if (in_beetwen_entry_space>=entries_needed)
					{
						return i+(e1->size/get_entry_size()+1);
					}
				}
				e2 = (entry_t *) list_get(entry_table,list_size(entry_table)-1);
				if(get_total_entries()- e2->number>=entries_needed)
				{
					return (e2->size/get_entry_size()+ 1) + e2->number;
				}
    		}
    	}

    }

    return -1;
}


bool entry_table_have_entries(key_value_t* key_value)
{
	return entry_table_entries_needed(key_value)<=entries_left;
}

int entry_table_entries_needed(key_value_t * key_value)
{
		int entries=(key_value->size/get_entry_size());
		return key_value->size%get_entry_size()==0?entries:entries+1;
	//return key_value->size/get_entry_size()+1;
}

entry_t * convert_key_value_t_to_entry_t(key_value_t * key_value){
	entry_t * entry = malloc(sizeof(entry_t));
	entry->size = key_value->size;
	entry->key = strdup(key_value->key);
	return entry;
}

bool entry_table_delete(key_value_t * key_value)
{
	for (int i=0; i<list_size(entry_table);i++)
	{
		entry_t * entry = (entry_t*) list_get(entry_table,i);
		if (!strcmp(key_value->key,entry->key))
		{
			list_remove(entry_table,i);
			entries_left+=entry_table_entries_needed(key_value);
			return true;
		}
	}
	return false;
}

void entry_table_print_table(){
	for (int i=0; i<list_size(entry_table);i++)
		{
		entry_t * entry=(entry_t *) list_get(entry_table,i);
		printf("Registro %d, KEY: %s, TAMANIO: %d y tiene INDICE STORAGE: %d \n",i,entry->key,entry->size,entry->number);
		}
}

int entry_table_atomic_entries_count()
{
	int atomic_entries=0;
	for (int i=0; i<list_size(entry_table);i++)
		{
			entry_t * entry = (entry_t*) list_get(entry_table,i);
			if (entry_table_is_entry_atomic(entry))
				atomic_entries+=1;

		}
	return atomic_entries;
}

bool entry_table_is_entry_atomic(entry_t * entry)
{
	return entry->size <= get_entry_size();
}

void entry_table_delete_few(t_list* keys){
	for(int i=0;i<list_size(keys);i++)
		{
		key_value_t* key_value= key_value_generator(list_get(keys,i),0);
			entry_table_delete(key_value);
		}
}



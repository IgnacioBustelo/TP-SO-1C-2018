#include <stdlib.h>

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
	if(next_entry>=0 &&list_add(entry_table,(void *)new_entry))
	{
		entries_left-=entry_table_entries_needed(key_value);
		list_sort(entry_table,ascending);
		return true;
	}

return false;
}

bool ascending(void * a, void *b){
	entry_t * e1 = (entry_t*)a;
	entry_t * e2 = (entry_t*)b;
	return e1->number>e2->number?false:true;
}

int entry_table_next_entry(key_value_t* key_value){

	int in_beetwen_entry_space=0;
	int entries_used=list_size(entry_table);

	int entries_needed = entry_table_entries_needed(key_value);



	entry_t* e1;
	entry_t* e2;

    if(entry_table_have_entries(key_value))
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
	return key_value->size/get_entry_size()+1;
//	int total = 0;
//	int size = key_value->size;
//
//	while (size > 0)
//			{
//				total++;
//				size -= get_entry_size();
//			}
//
//	return total;
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
			entries_left=+(entry->size/get_entry_size())+1;
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

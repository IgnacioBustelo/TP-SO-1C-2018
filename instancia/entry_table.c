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
	if(list_add(entry_table,(void *)new_entry))
	{
		entries_left--;
		return true;
	}
return false;
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
    			if (e1->number!=0)
				{
					if ((e1->number-1)>=entries_needed)
					{
						return 0;
					}
				}
				for (int i=0;i+1<entries_used;i++)
				{

					e1 = (entry_t*) list_get(entry_table,i);
					e2 = (entry_t*) list_get(entry_table,i+1);

					in_beetwen_entry_space = e2->number-e1->number;

					if (in_beetwen_entry_space>=entries_needed)
					{
						return i;
					}
				}
				e2 = (entry_t *) list_get(entry_table,list_size(entry_table)-1);
				if(get_total_entries()- e2->number)
				{
					return e2->size/get_entry_size()+ 1;
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

	int total = 0;
	int size = key_value->size;

	while (size > 0)
			{
				total++;
				size -= get_entry_size();
			}

	return total;
}

entry_t * convert_key_value_t_to_entry_t(key_value_t * key_value){
	entry_t * entry = malloc(sizeof(entry_t));
	entry->size = key_value->size;
	entry->key = strdup(key_value->key);
	return entry;
}

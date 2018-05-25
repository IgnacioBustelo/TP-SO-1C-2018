#include <stdlib.h>

#include "entry_table.h"
#include "globals.h"

void node_to_entry(entry_t * entry,void * node)
{
	entry = malloc(sizeof(entry_t));
	memcpy(&(entry),node,sizeof(entry_t));
}

void entry_table_init() {
	entry_table= list_create();
	entries_left = storage_setup->total_entries;
}

void entry_table_insert(int next_entry, key_value_t* key_value)//Solo guarda para un bloque!!!
{
	if (entries_left-->0)
	{
		list_add(entry_table,key_value);
	}

	//SORT DE LA LISTA
}

int entry_table_next_entry(key_value_t* key_value){

	size_t in_beetwen_entry_space=0;
	int entries_used=list_size(entry_table);
	int entries_needed=entry_table_entries_needed(key_value);

	entry_t* e1;
	entry_t* e2;

    if(entry_table_have_entries(key_value))
    {
    	if (entry_table!=NULL)
    	{
			for (int i=0;i+1<entries_used;i++)
			{
				node_to_entry(e1,list_get(entry_table,i));
				node_to_entry(e2,list_get(entry_table,i+1));

				in_beetwen_entry_space = e2->number-e1->number;

				if (in_beetwen_entry_space>=entries_needed)
				{
					return i;
				}
			}
    	}
    }

    return -1;
}

void entry_table_update(int next_entry, key_value_t* key_value) {
	// TODO: Implementar
}

bool entry_table_have_entries(key_value_t* key_value)
{
	return entry_table_entries_needed(key_value)<=entries_left?true:false;
}

int entry_table_entries_needed(key_value_t *key_value)
{

	int total=0;
	int size=key_value->size;

	while (size - storage_setup->entry_size > 0)
		{
			total++;
		}
	return total++;
}

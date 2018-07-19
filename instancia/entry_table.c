#include <commons/string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../libs/messenger.h"

#include "tests/utils.h"

#include "algorithms.h"
#include "entry_table.h"
#include "globals.h"

int _req_entries(int size) {
	int entries = size/get_entry_size();

	return size % get_entry_size() == 0 ? entries : entries + 1;
}

int entry_table_diff_entries(key_value_t* key_value) {
	int current_req = _req_entries(key_value->size);

	if(entry_table_has_key(key_value->key, false)) {
		entry_t* old_entry = entry_table_get_entry(key_value->key);

		current_req -= _req_entries(old_entry->size);
	}

	return current_req;
}

void entry_table_init() {
	entry_table = list_create();
	entries_left = get_total_entries();
}

bool entry_table_insert(int next_entry, key_value_t* key_value)
{
	entry_t* new_entry=convert_key_value_t_to_entry_t(key_value);
	entry_t* old_entry = entry_table_get_entry(key_value->key);
	new_entry->number=next_entry;
	if(next_entry>=0)
	{
		if(old_entry==NULL)
		{
			if (entry_table_get_entry(key_value->key)!=NULL)
			{
			key_value_t* kv_old = key_value_generator(entry_table_get_entry(key_value->key)->key,entry_table_get_entry(key_value->key)->size);
			entry_table_delete(kv_old);
			entry_table_status_delete_kv(kv_old);
			key_value_destroy(kv_old);
			}

			list_add(entry_table,(void *)new_entry);
			entries_left-=entry_table_entries_needed(key_value);
			list_sort(entry_table,ascending);


		}
	else
	{
		key_value_t* kv_old = key_value_generator(entry_table_get_entry(key_value->key)->key,entry_table_get_entry(key_value->key)->size);
		entries_left += entry_table_entries_needed(kv_old);

		if(next_entry== old_entry->number)
		{
			old_entry->size = new_entry->size;
		}
		else {
			old_entry->size = new_entry->size;
			old_entry->number = new_entry->number;
			list_sort(entry_table,ascending);
		}

		entries_left -= entry_table_entries_needed(key_value);
		key_value_destroy(kv_old);
		entry_table_key_value_destroy(new_entry);
	}
		//free(new_entry); COMO VAS A LIBERAR LO QUE ACABAS DE INSERTAR??????? ESTAS LOCO????
		return true;
	}
entry_table_key_value_destroy(new_entry);
return false;
}

t_list* entry_table_get_key_list()
{
	void* map(entry_t* entry) {
		return (void*) strdup(entry->key);
	}

	return (list_is_empty(entry_table)) ? list_create() : list_map(entry_table, (void*) map);
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
	list_destroy_and_destroy_elements(entry_table, (void*) entry_table_key_value_destroy);
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
			// TODO: Lo vas a usar? -> key_value_t * key_value_replaced = key_value_generator("X",extra_entries_needed*get_entry_size());
			if(extra_entries_needed<=entries_left)
			{
				int i=0;
				int number = entry_table_get_entry(key_value->key)->number+entry_table_entries_needed(existing_kv);
				while (entry_table_get_entry_by_entry_number(number+i)==NULL && i<extra_entries_needed)
				{
					i++;
				}
				if (i==extra_entries_needed)
				{
					key_value_destroy(existing_kv);

					return entry_table_get_entry(key_value->key)->number;

				}
			}
			key_value_destroy(existing_kv);
		}
	}
	 if(entry_table_have_entries(key_value) || entry_to_be_modified!=NULL)
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

					in_beetwen_entry_space = (e2->number)-(((e1->size%get_entry_size())==0?(e1->size/get_entry_size())+ e1->number:(e1->size/get_entry_size()+1)+ e1->number) );

					//messenger_show("WARNING", "%d) Necesito %d entradas : e%d key1: %s - inbs: %d - e%d key2: %s", i, entries_needed, e1->number, e1->key, in_beetwen_entry_space, e2->number, e2->key);

					if (in_beetwen_entry_space>=entries_needed)
					{
						return (e1->number+((e1->size%get_entry_size())==0?(e1->size/get_entry_size()):(e1->size/get_entry_size()+1)));
					}
				}
				e2 = (entry_t *) list_get(entry_table,list_size(entry_table)-1);
				if(get_total_entries() - (e2->number + _req_entries(e2->size)) >= entries_needed)
				//if(get_total_entries()- e2->number>entries_needed && e2->number+entries_needed<get_total_entries())
				{
					return ((e2->size%get_entry_size())==0?(e2->size/get_entry_size())+ e2->number:(e2->size/get_entry_size()+1)+ e2->number) ;
				}
    		}
    	}

    }

    return -1;
}

bool entry_table_has_entries(key_value_t* key_value) {
	return entry_table_diff_entries(key_value) <= entries_left;
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

bool entry_table_delete(key_value_t * key_value)//TODO: GUARDA ACA DEBO BUSCAR EL TAMANIO A ELIMINAR DE LA ENTRIE
{
	for (int i=0; i<list_size(entry_table);i++)
	{
		entry_t * entry = (entry_t*) list_get(entry_table,i);
		if (!strcmp(key_value->key,entry->key))
		{
			entry_t* entry = entry_table_get_entry(key_value->key);
			key_value_t* kv_old= key_value_generator(entry->key,entry->size);
			list_remove_and_destroy_element(entry_table,i, (void*) entry_table_key_value_destroy);
			entries_left+=entry_table_entries_needed(kv_old);
			key_value_destroy(kv_old);
			return true;
		}
	}
	return false;
}

void entry_table_print_table(){
	for (int i=0; i<list_size(entry_table);i++)
		{
		entry_t * entry=(entry_t *) list_get(entry_table,i);
		messenger_show("INFO", "Clave %s: - Tamanio: %d - Indice Storage: %d",entry->key, entry->size, entry->number  );
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
		key_value_destroy(key_value);
		}
}

void entry_table_show() {
	if(list_is_empty(entry_table)) {
		messenger_show("INFO", "La tabla de entradas esta vacia");
	}

	void _read(entry_t* entry) {
		bool _get_lru(status_t* status) {
			return string_equals_ignore_case(status->key, entry->key);
		}

		status_t* entry_status = (status_t*) list_find(entry_table_status_global, (void*) _get_lru);

		messenger_show("INFO", "Entrada: %-2d - Tamano: %-2d - Entradas ocupadas: %-2d - Ultima referencia: %-2d - Clave: %-40s", entry->number, entry->size, _req_entries(entry->size), entry_status->last_referenced, entry->key);
	}

	t_list* sorted_list = list_duplicate(entry_table);

	list_sort(sorted_list, (void*) ascending);

	list_iterate(sorted_list, (void*) _read);

	list_destroy(sorted_list);
}

bool entry_table_has_continous_entries(key_value_t* key_value)
{
	t_list* mock_entry_table = list_duplicate(entry_table);

	if(entry_table_has_key(key_value->key, false)) {
		bool _equals(entry_t* entry) {
			return string_equals_ignore_case(key_value->key, entry->key);
		}

		entry_t* current_entry = list_remove_by_condition(mock_entry_table, (void*) _equals), *following_entry;

		if(list_is_empty(mock_entry_table)) {
			list_destroy(mock_entry_table);
			return true;
		}

		int h = 0;
		do
		{
			following_entry = (entry_t*) list_get(mock_entry_table, h);
			h++;
		}
		while(following_entry->number < current_entry->number && h < list_size(mock_entry_table));

		if(following_entry->number < current_entry->number && get_total_entries() - current_entry->number + _req_entries(key_value->size)) {
			list_destroy(mock_entry_table);
			return true;
		}

		if(current_entry->number + _req_entries(current_entry->size) + entry_table_diff_entries(key_value) < following_entry->number) {
			list_destroy(mock_entry_table);
			return true;
		}
	}

	int entries_needed = _req_entries(key_value->size);
	int in_beetwen_entry_space=0;
		int entries_used=list_size(mock_entry_table);
		entry_t* e1;
		entry_t* e2;
	if (entry_table!=NULL )
	    	{
	    		if(entries_used==0) {
	    			list_destroy(mock_entry_table);
	    			return 1;
	    		}
	    		else
	    		{

	    			e1 = (entry_t*) list_get(mock_entry_table,0);

	    			if (e1->number>0)
					{
						if ((e1->number)>=entries_needed)
						{
							list_destroy(mock_entry_table);
							return true;
						}
					}
					for (int i=0;i+1<entries_used;i++)
					{
						e1 = (entry_t*) list_get(mock_entry_table,i);
						e2 = (entry_t*) list_get(mock_entry_table,i+1);

						in_beetwen_entry_space = (e2->number)-(((e1->size%get_entry_size())==0?(e1->size/get_entry_size())+ e1->number:(e1->size/get_entry_size()+1)+ e1->number) );

						messenger_show("WARNING", "Espacio entre medio de %d que ocupa %d y %d es %d cuando falta %d", e1->number, _req_entries(e1->size), e2->number, in_beetwen_entry_space, entries_needed);

						if (in_beetwen_entry_space>=entries_needed)
						{
							list_destroy(mock_entry_table);
							return true;
						}
					}
					e2 = (entry_t *) list_get(mock_entry_table,list_size(mock_entry_table)-1);
					messenger_show("WARNING", "Espacio entre tope (%d) y entrada %d que ocupa %d = %d >= Necesario: %d", get_total_entries(), e2->number, (e2->number + _req_entries(e2->size)), get_total_entries() - (e2->number + _req_entries(e2->size)), entries_needed);
					if(get_total_entries() - (e2->number + _req_entries(e2->size)) >= entries_needed)
					{
						list_destroy(mock_entry_table);
						return true;
					}
	    		}
	    		messenger_show("WARNING", "No entra");
	    		list_destroy(mock_entry_table);
	    		return false;
	    	}
	else {
		list_destroy(mock_entry_table);
		return false;
	}
}

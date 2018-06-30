#ifndef INSTANCIA_MISC_PAGE_TABLE_H_
#define INSTANCIA_MISC_PAGE_TABLE_H_

#include <commons/collections/dictionary.h>
#include <stdlib.h>

#include "entry.h"

t_dictionary* page_table;

void		page_table_init();

entry_t*	page_table_select(char* key);

void		page_table_insert(char* key, entry_t* new_entry);

void		page_table_update(char* key, entry_t* updated_entry);

void		page_table_delete(char* key);

bool		page_table_exists_key(char* key);

void		page_table_show();

void		page_table_destroy();

#endif

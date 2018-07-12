#ifndef INSTANCIA_ENTRY_TABLE_H_
#define INSTANCIA_ENTRY_TABLE_H_

#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>
#include "globals.h"

// Estructuras de datos

enum{
	ENTRY_LIMIT_ERROR = -1,
	ENTRY_UPDATE_OK,
	ENTRY_INSERT_OK,
	ENTRY_UPDATE_ERROR
};

typedef struct{
	size_t	number;
	size_t	size;
	char * key;
} entry_t;

// Atributos

t_list * entry_table;

int entries_left;

// Interfaz

void		entry_table_init();

void		entry_table_destroy();

bool    	entry_table_insert(int next_entry, key_value_t* key_value);

entry_t*	entry_table_get_entry(char* key);

int			entry_table_next_entry(key_value_t* key_value);

bool 		entry_table_have_entries(key_value_t* key_value);

int     	entry_table_entries_needed(key_value_t * key_value);

bool		entry_table_delete(key_value_t * key_value);

void		entry_table_print_table();

int 		entry_table_atomic_entries_count();

bool	    entry_table_is_entry_atomic(entry_t * entry);

entry_t* 	entry_table_get_entry_by_entry_number(int number);

void		entry_table_delete_few(t_list* keys);

void		entry_table_key_value_destroy(entry_t * entry);

bool		entry_table_has_key(char* key,bool is_new);

t_list*		entry_table_get_key_list();

void		entry_table_show();

bool		ascending(void * a, void *b);

entry_t *	convert_key_value_t_to_entry_t(key_value_t * key_value);

#endif

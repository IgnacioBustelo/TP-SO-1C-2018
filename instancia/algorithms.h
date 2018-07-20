#ifndef INSTANCIA_ALGORITHMS_H_
#define INSTANCIA_ALGORITHMS_H_

#include <commons/collections/list.h>
#include <stdio.h>
#include "entry_table.h"
#include "tests/utils.h"

enum {
	ATOMIC,
	NON_ATOMIC,
	FREE,
	USED
} entry_status;

typedef struct{
	int status;
	char * key;
	int last_referenced;
	int space_used;
} status_t;

t_list* entry_table_status_global;
int algorithm_circular_pointer;


int			algorithms_exec(char algorithm_id, t_list* entry_table, key_value_t* key_value, t_list* replaced_keys);

int			algorithm_circular(t_list* entry_table,key_value_t* key_value, t_list* replaced_keys);

int			algorithm_lru(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys);

int			algorithm_bsu(t_list* entry_table,key_value_t* key_value,t_list* replaced_keys);

bool		new_value_fits_with_replaced(key_value_t* key_value);

bool		new_value_fits(key_value_t* key_value); // Se fija si el tamanio del nuevo key_value entra entre todas las entradas atomicas que hay y los lugares libres dentro de la tabla

status_t*	convert_entry_t_to_status_t(entry_t * entry);

void		entry_table_status_print_table();

t_list*		original_entry_table_migration_to_entry_table_status();

void		algorithm_circular_set_pointer(int index);

void 		entry_table_status_last_referenced_add_all();

void 		entry_table_status_init();

void 		entry_table_status_add_kv(key_value_t* key_value,int number);

void 		entry_table_status_delete_kv(key_value_t* key_value);

bool		entry_table_status_continuous_entries(t_list* replaced_keys);

int			entry_table_status_find_and_get_index(char * key);

void		entry_table_status_destroy();

void		entry_table_status_show();

#endif

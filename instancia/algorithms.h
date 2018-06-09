#ifndef INSTANCIA_ALGORITHMS_H_
#define INSTANCIA_ALGORITHMS_H_

#include <commons/collections/list.h>
#include <stdio.h>
#include "entry_table.h"

enum {
	ATOMIC,
	NON_ATOMIC,
	FREE
} entry_status;

typedef struct{
	int status;
	char * key;
} status_t;

int algorithm_circular_pointer;

int algorithm_circular(t_list* entry_table,key_value_t* key_value, t_list* replaced_keys);

void algorithm_lru(t_list* entry_table, t_list* replaced_keys);

static bool new_value_fits(key_value_t* key_value);//Se fija si el tamanio del nuevo key_value entra entre todas las entradas atomicas que hay y los lugares libres dentro de la tabla

static status_t * convert_entry_t_to_status_t(entry_t * entry);

void entry_table_status_print_table(t_list* entry_table_status);

t_list* original_entry_table_migration_to_complete_one();

void    algorithm_circular_set_pointer(int index);

#endif

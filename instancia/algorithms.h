#ifndef INSTANCIA_ALGORITHMS_H_
#define INSTANCIA_ALGORITHMS_H_

#include <commons/collections/list.h>

#include "entry_table.h"

int algorithm_circular_pointer;

int algorithm_circular(t_list* entry_table, t_list* replaced_keys);

void algorithm_lru(t_list* entry_table, t_list* replaced_keys);

#endif

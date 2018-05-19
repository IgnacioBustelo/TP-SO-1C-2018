#ifndef INSTANCIA_ALGORITHMS_H_
#define INSTANCIA_ALGORITHMS_H_

#include <commons/collections/dictionary.h>

void algorithm_circular(t_dictionary* entry_table, t_dictionary* storage);

void algorithm_lru(t_dictionary* entry_table, t_dictionary* storage);

#endif

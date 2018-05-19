#ifndef INSTANCIA_TESTS_UTILS_H_
#define INSTANCIA_TESTS_UTILS_H_

#include <commons/collections/list.h>
#include <stdlib.h>

#include "../entry_table.h"
#include "../globals.h"

// Utilidades para Key Values

void			key_value_input(t_list* key_value_list, int from, int argc, char* argv[]);

key_value_t*	key_value_generator(char key[40], size_t size);

size_t			entry_table_required_entries(size_t size);

void			key_value_print(key_value_t* key_value);

void			print_entry(char* key, entry_t* entry);

void			print_ordered_stored_values();

#endif

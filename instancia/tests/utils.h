#ifndef INSTANCIA_TESTS_UTILS_H_
#define INSTANCIA_TESTS_UTILS_H_

#include <commons/collections/list.h>
#include <stdlib.h>

#include "../entry_table.h"
#include "../instancia.h"

key_value_t* key_value_create(char* key, char* value);

void key_value_input(t_list* key_value_list, int from, int argc, char* argv[]);

void key_value_destroy(key_value_t* key_value);

key_value_t* value_generator(char key[40], size_t size);

size_t required_entries(size_t size);

void print_key_value(key_value_t* key_value);

void print_entry(char* key, entry_t* entry);

void print_ordered_stored_values();

#endif

#ifndef INSTANCIA_STORAGE_H_
#define INSTANCIA_STORAGE_H_

#include <stdlib.h>

#include "instancia.h"

int storage_init(); // Lo hace Santi

int storage_next_entry(size_t key_size); // Lo hace Nico

int storage_set(key_value_t* key_value); // Lo hace Santi

void storage_destroy(); // Lo hace Nico


#endif /* INSTANCIA_STORAGE_H_ */

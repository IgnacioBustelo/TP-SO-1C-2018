#ifndef INSTANCIA_STORAGE_H_
#define INSTANCIA_STORAGE_H_

#include <stdlib.h>

#include "instancia.h"

typedef enum {
	STRG_INIT_SUCCESS,
	STRG_INIT_CREATE_ERROR
} storage_status;

int storage_init(); // Lo hace Santi

int storage_next_entry(size_t key_size); // Lo hace Nico

int storage_set(key_value_t* key_value); // Lo hace Santi

int storage_update(key_value_t* key_value); //Nico

void storage_destroy(); // Lo hace Nico


#endif /* INSTANCIA_STORAGE_H_ */

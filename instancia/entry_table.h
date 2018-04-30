#ifndef INSTANCIA_ENTRY_TABLE_H_
#define INSTANCIA_ENTRY_TABLE_H_

#include <stdlib.h>

#include "instancia.h"

// Estructuras de datos

// Entrada para la tabla de entradas

typedef struct{
	size_t	number;
	size_t	size;
} entry_t;

// Códigos de éxito para las operaciones sobre la tabla de entradas

typedef enum {
	ET_INIT_SUCCESS,
	ET_INIT_ERROR,
	ET_INSERT_SUCCESS
} entry_status;

int entry_table_init(); // Lo hace Santi

int entry_table_insert(key_value_t* key_value); // Lo hace Santi

int entry_table_update(key_value_t* key_value); // Lo hace Nico

int entry_table_delete(char* key); // Lo hace Nico

#endif

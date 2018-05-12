#ifndef INSTANCIA_ENTRY_TABLE_H_
#define INSTANCIA_ENTRY_TABLE_H_

#include <commons/collections/dictionary.h>
#include <stdlib.h>

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
} entry_t;

// Atributos

t_dictionary* entry_table;

size_t entries_left;

// Interfaz

void	entry_table_init(); // Lo hace Santi

int		entry_table_next_entry(key_value_t* key_value);

bool	entry_table_have_entries(key_value_t* key_value); // Lo hace Nico

void	entry_table_insert(int next_entry, key_value_t* key_value); // Lo hace Santi

void	entry_table_update(int next_entry, key_value_t* key_value); // Lo hace Nico

void	entry_table_delete(char* key); // Lo hace Nico

void	entry_table_destroy();

#endif

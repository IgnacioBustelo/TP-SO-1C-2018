#ifndef INSTANCIA_STORAGE_H_
#define INSTANCIA_STORAGE_H_

#include <commons/collections/dictionary.h>

#include "globals.h"

enum{
	STORAGE_ERROR=-1;
};

// Atributos

t_dictionary* storage;

// Interfaz

void	storage_init(); // Lo hace Santi

void	storage_set(int next_entry, key_value_t* key_value); // Lo hace Santi

void	storage_store(int entry, size_t value_size); // Lo hace Santi

void	storage_destroy(); // Lo hace Nico

#endif

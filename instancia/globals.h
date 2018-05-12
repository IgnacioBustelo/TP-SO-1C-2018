#ifndef INSTANCIA_TESTS_GLOBALS_H_
#define INSTANCIA_TESTS_GLOBALS_H_

#include <commons/collections/dictionary.h>
#include <stdlib.h>

// Estructuras de datos

typedef struct {
	char*	key;
	char*	value;
	size_t	size;
} key_value_t;

typedef struct {
	size_t	total_entries;
	size_t	entry_size;
} storage_setup_t;

// Variables globales

storage_setup_t storage_setup;

// Funciones auxiliares

key_value_t* key_value_create(char* key, char* value);

void key_value_destroy(key_value_t* key_value);

int get_total_entries();

int get_entry_size();

#endif

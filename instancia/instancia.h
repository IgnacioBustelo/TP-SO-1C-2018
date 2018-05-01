#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

// Versión nueva

#include <commons/collections/dictionary.h>
#include <stdlib.h>



// Estructuras de datos

// Acá guardamos lo que recibimos del Coordinador.

typedef struct {
	char*	key;
	char*	value;
	size_t	size;
} key_value_t;



// Variables globales

t_dictionary* entry_table;

t_dictionary* storage;

size_t total_entries;

size_t entry_size;

int fd_instancia;

int fd_coordinador;



// Interfaz

void instance_set();


// Versión vieja



#define KEY_SIZE 12

#define FREE_SPACE_LEFT 10
#define NOT_FREE_SPACE_LEFT 11

void exit_gracefully(int status);





typedef struct {
	char* key;
	int entrie_number;
	int entrie_size;
}data_saving_administrative_structure_t;

typedef struct {
	int entrie_number;
	char* entrie_data;
}data_saving_structure_t;

#endif

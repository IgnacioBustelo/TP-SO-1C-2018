#ifndef INSTANCIA_STORAGE_H_
#define INSTANCIA_STORAGE_H_

#include "globals.h"

typedef struct {
	char** data;
	size_t frames;
	size_t frame_size;
} storage_t;

enum{
	STORAGE_ERROR=-1
};

// Atributos

storage_t* storage;

// Interfaz

void	storage_init(size_t frames, size_t size); // Lo hace Santi

void	storage_set(int next_entry, char* value, size_t size); // Lo hace Santi

void	storage_store(int entry, size_t value_size); // Lo hace Santi

void	storage_show();

void	storage_destroy(); // Lo hace Nico

#endif

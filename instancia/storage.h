#ifndef INSTANCIA_STORAGE_H_
#define INSTANCIA_STORAGE_H_

#include "globals.h"

typedef struct {
	size_t	entries;
	size_t	entry_size;
	void*	data;
} storage_t;

enum{
	STORAGE_ERROR=-1
};

// Atributos

storage_t* storage;

// Interfaz

void	storage_init(size_t entries, size_t entry_size);

void	storage_set(int next_entry, void* value, size_t size);

void*	storage_retrieve(int entry, size_t value_size);

int		storage_required_entries(int size);

void	storage_show();

void	storage_destroy();

#endif

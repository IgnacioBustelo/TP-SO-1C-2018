#ifndef INSTANCIA_STORAGE_H_
#define INSTANCIA_STORAGE_H_

#include "globals.h"

typedef struct {
	char*	mount_point;
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

void	storage_init(char* mount_point, size_t entries, size_t entry_size); // Lo hace Santi

void	storage_set(int next_entry, void* value, size_t size); // Lo hace Santi

void	storage_store(int entry, char* key, size_t value_size); // Lo hace Santi

int		storage_required_entries(int size);

void	storage_show();

void	storage_destroy(); // Lo hace Nico

#endif

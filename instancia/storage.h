#ifndef INSTANCIA_STORAGE_H_
#define INSTANCIA_STORAGE_H_

#include "globals.h"

// Resultados de operaciones

typedef enum {
	STRG_SUCCESS = 0,
	STRG_ERROR_INIT = -1,
	STRG_ERROR_SET = -2
} storage_status;

typedef struct {
	size_t	entries;
	size_t	entry_size;
	void*	data;
} storage_t;

// Atributos

storage_t* storage;

#define STRG_BASE	storage->data
#define STRG_LIMIT	storage->data + (storage->entries * storage->entry_size)

// Interfaz

int		storage_init(size_t entries, size_t entry_size);

int		storage_set(int next_entry, void* value, size_t size);

void*	storage_retrieve(int entry, size_t value_size);

int		storage_required_entries(int size);

void	storage_show();

void	storage_destroy();

#endif

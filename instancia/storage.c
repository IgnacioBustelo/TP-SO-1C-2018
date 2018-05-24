#include <commons/string.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/logger.h"
#include "globals.h"
#include "storage.h"

static char* value_to_string(void* value, size_t size) {
	char* string_value = malloc(size + 1);

	memset(string_value, '\0', size + 1);
	memcpy(string_value, value, size);

	return string_value;
}

static void insert_value(int next_entry, void* value, size_t size) {
	memcpy(storage->data[next_entry], value, size);

	char* string_value = value_to_string(value, size);

	messenger_show("DEBUG", "Insercion de valor '%s' de tamanio %d en la entrada %d", string_value, size, next_entry);

	free(string_value);
}

void storage_init(size_t entries, size_t entry_size) {
	messenger_show("DEBUG", "Inicio del Storage");

	storage = malloc(sizeof(storage_t));

	storage->entries = entries;
	storage->entry_size = entry_size;
	storage->data = calloc(storage->entries, sizeof(void*));

	int i;
	for(i = 0; i < storage->entries; i++) {
		storage->data[i] = malloc(storage->entry_size);
		memset(storage->data[i], 0, storage->entry_size);
	}

	messenger_show("DEBUG", "Inicio exitoso del Storage con %d entradas de tamano %d", entries, entry_size);
}

void storage_set(int next_entry, void* value, size_t size) {
	if(size <= storage->entry_size) {
		insert_value(next_entry, value, size);
	}

	else {
		int remainder = size - storage->entry_size;

		void* fitting_value = malloc(size), *remainder_value = malloc(remainder);

		memcpy(fitting_value, value, storage->entry_size);
		memcpy(remainder_value, value + storage->entry_size, remainder);

		insert_value(next_entry, fitting_value, storage->entry_size);

		storage_set(++next_entry, remainder_value, remainder);

		free(fitting_value);
		free(remainder_value);
	}
}

void storage_show() {
	messenger_show("INFO", "Muestra de valores almacenados en el Storage");

	int i;

	for(i = 0; i < storage->entries; i++) {
		char* value = value_to_string(storage->data[i], storage->entry_size);

		char* message = string_duplicate("Entrada %d : %s");

		messenger_show("INFO", message, i,  value);

		free(message);

		free(value);
	}
}

void storage_destroy() {
	messenger_show("DEBUG", "Liberacion del Storage");

	int i;

	for(i = 0; i < storage->entries; i++) {
		free(storage->data[i]);
	}

	free(storage->data);
	free(storage);

	messenger_show("DEBUG", "Liberacion exitosa del Storage");
}

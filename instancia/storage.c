#include <commons/string.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/messenger.h"
#include "globals.h"
#include "storage.h"

static void insert_value(int next_entry, void* value, size_t size) {
	memcpy(storage->data + (next_entry * storage->entry_size), value, size);

	char* string_value = value_to_string(value, size);

	messenger_show("DEBUG", "Insercion de valor '%s' de tamanio %d en la entrada %d", string_value, size, next_entry);

	free(string_value);
}

void storage_init(size_t entries, size_t entry_size) {
	messenger_show("DEBUG", "Inicio del Storage");

	storage = malloc(sizeof(storage_t));

	storage->entries = entries;
	storage->entry_size = entry_size;
	storage->data = calloc(storage->entries, entry_size);

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

void* storage_retrieve(int entry, size_t value_size) {
	void* data = malloc(value_size);

	memcpy(data, storage->data + (entry * storage->entry_size), value_size);

	return data;
}

void storage_show() {
	messenger_show("INFO", "Muestra de valores almacenados en el Storage");

	int i, longest_size = messenger_longest_string_length(storage->entries);

	for(i = 0; i < storage->entries; i++) {
		void* data = storage_retrieve(i, storage->entry_size);

		char* value = value_to_string(data, storage->entry_size);

		messenger_show("INFO", "[%p] - Entrada %.*d: %s", storage->data + (i * storage->entry_size), longest_size, i, value);

		free(data);

		free(value);
	}
}

void storage_destroy() {
	messenger_show("DEBUG", "Liberacion del Storage");

	free(storage->data);
	free(storage);

	messenger_show("DEBUG", "Liberacion exitosa del Storage");
}

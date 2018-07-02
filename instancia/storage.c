#include <commons/string.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/messenger.h"
#include "globals.h"
#include "storage.h"

int storage_init(size_t entries, size_t entry_size) {
	messenger_show("DEBUG", "Inicio del Storage");

	storage = malloc(sizeof(storage_t));

	if(storage == NULL) {
		messenger_show("ERROR", "Error creando la estructura para el Storage");

		return STRG_ERROR_INIT;
	}

	storage->entries = entries;
	storage->entry_size = entry_size;
	storage->data = calloc(storage->entries, entry_size);

	if(storage->data == NULL) {
		messenger_show("ERROR", "Error reservando memoria para alojar datos en el Storage");

		return STRG_ERROR_INIT;
	}

	messenger_show("DEBUG", "Inicio exitoso del Storage con %d entradas de tamano %d desde la posicion [%p] a [%p]", entries, entry_size, STRG_BASE, STRG_LIMIT);

	return STRG_SUCCESS;
}

int storage_set(int next_entry, void* value, size_t size) {
	char* string_value = value_to_string(value, size);

	void* pointer = STRG_BASE + (next_entry * storage->entry_size);

	if(pointer + size > STRG_LIMIT) {
		messenger_show("ERROR", "Error insertando el valor '%s' en la entrada %d porque ocupa %d entrada/s y escribe hasta la posicion [%p]", string_value, next_entry, storage_required_entries(size), pointer + size);

		free(string_value);

		return STRG_ERROR_SET;
	}

	memcpy(pointer, value, size);

	messenger_show("DEBUG", "Insercion de valor '%s' de tamanio %d en la entrada %d, ocupando %d entrada/s", string_value, size, next_entry, storage_required_entries(size));

	free(string_value);

	return STRG_SUCCESS;
}

void* storage_retrieve(int entry, size_t value_size) {
	void* data = malloc(value_size);

	memcpy(data, storage->data + (entry * storage->entry_size), value_size);

	return data;
}

char* storage_retrieve_string(int entry, size_t value_size) {
	void* data = storage_retrieve(entry, value_size);

	char* string = messenger_bytes_to_string(data, value_size);

	free(data);

	return string;
}

int storage_required_entries(int size) {
	if(size < storage->entry_size) {
		return 1;
	}

	else {
		int required = size/storage->entry_size;

		return (size % storage->entry_size == 0) ? required : ++required;
	}
}

void storage_show() {
	messenger_show("INFO", "Muestra de valores almacenados en el Storage desde [%p] a [%p]", STRG_BASE, STRG_LIMIT);

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

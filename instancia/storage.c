#include <commons/string.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/logger.h"
#include "globals.h"
#include "storage.h"

void storage_init(size_t entries, size_t entry_size) {
	messenger_show("DEBUG", "Inicio del Storage");

	storage = malloc(sizeof(storage_t));

	storage->entries = entries;
	storage->entry_size = entry_size;
	storage->data = calloc(storage->entries, sizeof(char*));

	int i;
	for(i = 0; i < entries; i++) {
		storage->data[i] = string_repeat('\0', storage->entry_size - 1);
	}

	messenger_show("DEBUG", "Inicio exitoso del Storage con %d frames de tamano %d", entries, entry_size);
}

void storage_set(int next_entry, char* value, size_t size) {
	if(size <= storage->entry_size) {
		messenger_show("DEBUG", "Insercion de valor atomico '%s' de tamanio %d en la entrada %d", value, size, next_entry);

		memcpy(storage->data[next_entry], value, size);
	}

	else {
		size_t string_size = storage->entry_size - 1;

		char* fitting_value = string_substring_until(value, string_size);
		char* remainder_value = string_substring_from(value, string_size);

		messenger_show("DEBUG", "Insercion de valor no atomico '%s' en la entrada %d", fitting_value, next_entry);

		memcpy(storage->data[next_entry], fitting_value, string_size);

		storage_set(++next_entry, remainder_value, size -= string_size);

		free(fitting_value);
		free(remainder_value);
	}
}

void storage_show() {
	messenger_show("INFO", "Muestra de valores almacenados en el Storage");

	int i;

	for(i = 0; i < storage->entries; i++) {
		messenger_show("INFO", "Entrada %d : %s", i, storage->data[i]);
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

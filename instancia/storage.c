#include <commons/string.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/logger.h"
#include "globals.h"
#include "storage.h"

void storage_init(size_t frames, size_t frame_size) {
	messenger_show("DEBUG", "Inicio del Storage");

	storage = malloc(sizeof(storage_t));

	storage->data = calloc(frames, frame_size);
	storage->frames = frames;
	storage->frame_size = frame_size;

	messenger_show("DEBUG", "Inicio exitoso del Storage");
}

void storage_set(int next_entry, char* value, size_t size) {
	if(size <= storage->frame_size) {
		memcpy(storage->data[next_entry], value, size);

		messenger_show("DEBUG", "Insercion de valor atomico %s en la entrada %d", value, next_entry);
	}

	else {
		char* fitting_value = string_substring_until(value, storage->frame_size);
		char* remainder_value = string_substring_from(value, storage->frame_size);

		memcpy(storage->data[next_entry], fitting_value, storage->frame_size);

		messenger_show("DEBUG", "Insercion de valor no atomico %s en la entrada %d", fitting_value, next_entry);

		storage_set(++next_entry, remainder_value, size -= storage->frame_size);

		free(fitting_value);
		free(remainder_value);
	}
}

void storage_show() {
	messenger_show("INFO", "Muestra de valores almacenados en el Storage");

	int i;

	for(i = 0; i < storage->frames; i++) {
		messenger_show("INFO", "Entrada %d : %s", i, storage[i]);
	}
}

void storage_destroy() {
	messenger_show("DEBUG", "Liberacion del Storage");

	int i;

	for(i = 0; i < storage->frames; i++) {
		free(storage->data[i]);
	}

	free(storage->data);
	free(storage);

	messenger_show("DEBUG", "Liberacion exitosa del Storage");
}

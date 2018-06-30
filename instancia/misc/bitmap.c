#include <commons/string.h>

#include "../../libs/messenger.h"

#include "bitmap.h"

void bitmap_init(int size) {
	bitmap = malloc(sizeof(bitmap_t));

	bitmap->size = size;
	bitmap->bit_vector = calloc(bitmap->size, sizeof(char*));

	messenger_show("INFO", "Iniciado el Bitmap de %d entradas", bitmap->size);
}

int bitmap_next_index(int size) {
	int i, free_spaces = 0;

	if(size < 1) {
		messenger_show("WARNING", "Se esta tratando de insertar algo que no ocupa espacio");

		return -1;
	}

	for(i = 0; i < bitmap->size; i++) {
		if(bitmap->bit_vector[i] == NULL) {
			free_spaces++;

			if(free_spaces == size) {
				int next_index = i + 1 - size;

				messenger_show("DEBUG", "Hay espacio contiguo suficiente para insertar el valor en la entrada %d", next_index);

				return next_index;
			}
		}

		else {
			free_spaces = 0;
		}
	}

	messenger_show("WARNING", "No hay espacio contiguo suficiente para ingresar %d entradas", size);

	return -1;
}

bool bitmap_is_insertable(int from_index, size_t size) {
	int i, free_indexes = 0;
	for(i = from_index; bitmap->bit_vector[i] == NULL && free_indexes < size; i++) {
		free_indexes++;
	}

	return free_indexes == size;
}

void bitmap_insert(int index, char* key, size_t size) {
	int i;
	for(i = 0; i < size; i++) {
		bitmap->bit_vector[i + index] = key;
	}
}

int bitmap_update(int index, char* key, size_t new_size, size_t old_size) {
	if(new_size < 1) {
		messenger_show("ERROR", "Para insertar algo de tamanio 0, es mejor hacer un DELETE");

		return -1;
	}

	if(new_size == old_size) {
		messenger_show("DEBUG", "El nuevo valor ocupa la misma cantidad de entradas");

		return index;
	}

	else if(new_size < old_size) {
		size_t removed_size = old_size - new_size;

		bitmap_delete(index + new_size, removed_size);

		messenger_show("DEBUG", "El nuevo valor ocupa %d entrada/s menos que antes", removed_size);

		return index;
	}

	else {
		size_t extra_size = new_size - old_size;

		if(bitmap_is_insertable(index + old_size, extra_size)) {
			bitmap_insert(index + old_size, key, extra_size);

			messenger_show("DEBUG", "El nuevo valor ocupa %d entrada/s mas que antes", extra_size);

			return index;
		}

		else {
			messenger_show("DEBUG", "El nuevo valor se tiene que desplazar dado que sobreescribe el valor de otra clave");

			bitmap_delete(index, old_size);

			int new_index = bitmap_next_index(new_size);

			if(new_index == -1) {
				bitmap_insert(index, key, old_size);

				return -1;
			}

			bitmap_insert(new_index, key, new_size);

			messenger_show("DEBUG", "El nuevo valor se desplazo a la entrada %d", new_index);

			return new_index;
		}
	}
}

void bitmap_delete(int index, size_t size) {
	int i;
	for(i = 0; i < size; i++) {
		bitmap->bit_vector[i + index] = NULL;
	}
}

void bitmap_show() {
	int i, max_index_string_length = messenger_longest_string_length(bitmap->size);

	messenger_show("INFO", "Muestra de Bitmap con claves de la Instancia");

	for(i = 0; i < bitmap->size; i++) {
		messenger_show("DEBUG", "Entrada %.*d: %s", max_index_string_length, i, (bitmap->bit_vector[i] != NULL) ? bitmap->bit_vector[i] : "");
	}
}

void bitmap_destroy() {
	messenger_show("INFO", "Liberando el Bitmap de la Instancia");

	free(bitmap->bit_vector);

	free(bitmap);

	messenger_show("INFO", "Se libero el Bitmap de la Instancia");
}

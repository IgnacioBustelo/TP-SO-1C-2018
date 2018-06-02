#include <commons/string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../libs/messenger.h"
#include "globals.h"
#include "storage.h"

static char* value_to_string(void* value, size_t size) {
	char* string_value = malloc(size + 1);

	memset(string_value, '\0', size + 1);
	memcpy(string_value, value, size);

	return string_value;
}

static void insert_value(int next_entry, void* value, size_t size) {
	memcpy(storage->data + (next_entry * storage->entry_size), value, size);

	char* string_value = value_to_string(value, size);

	messenger_show("DEBUG", "Insercion de valor '%s' de tamanio %d en la entrada %d", string_value, size, next_entry);

	free(string_value);
}

void storage_init(char* mount_point, size_t entries, size_t entry_size) {
	messenger_show("DEBUG", "Inicio del Storage");

	storage = malloc(sizeof(storage_t));

	storage->mount_point = mount_point;
	storage->entries = entries;
	storage->entry_size = entry_size;
	storage->data = calloc(storage->entries, entry_size);

	struct stat storage_stat = {0};

	if (stat(storage->mount_point, &storage_stat) == -1) {
	    mkdir(storage->mount_point, S_IRWXU);
	}

	messenger_show("DEBUG", "Inicio exitoso del Storage con %d entradas de tamano %d, en el punto de montaje %s", entries, entry_size, mount_point);
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

void storage_store(int entry, char* key, size_t value_size) {
	char* file_name = string_from_format("%s%s.txt", storage->mount_point, key);

	int key_fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);

	ftruncate(key_fd, value_size);

	void* mapped_memory = mmap(NULL, value_size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, key_fd, 0);

	memcpy(mapped_memory, storage->data + (entry * storage->entry_size), value_size);

	munmap(mapped_memory, value_size);

	close(key_fd);

	free(file_name);

	char* value = messenger_bytes_to_string(storage->data + (entry * storage->entry_size), value_size);

	messenger_show("INFO", "Se ejecuto un STORE de la clave %s con el valor %s", key, value);

	free(value);
}

void storage_show() {
	messenger_show("INFO", "Muestra de valores almacenados en el Storage");

	int i, longest_size = messenger_longest_string_length(storage->entries);

	for(i = 0; i < storage->entries; i++) {
		char* value = value_to_string(storage->data + (i * storage->entry_size), storage->entry_size);

		messenger_show("INFO", "[%p] - Entrada %.*d: %s", storage->data + (i * storage->entry_size), longest_size, i, value);

		free(value);
	}
}

void storage_destroy() {
	messenger_show("DEBUG", "Liberacion del Storage");

	free(storage->data);
	free(storage);

	messenger_show("DEBUG", "Liberacion exitosa del Storage");
}

#include <commons/string.h>
#include <stdlib.h>

#include "../../libs/messenger.h"
#include "entry.h"

entry_t* entry_new(int id, size_t size) {
	entry_t* new_entry = malloc(sizeof(entry_t));

	new_entry->id = id;
	new_entry->size = size;

	return new_entry;
}

bool entry_is_atomic(entry_t* entry, size_t atomic_size) {
	return entry->size <= atomic_size;
}

void entry_show(char* key, entry_t* entry) {
	messenger_show("INFO", "Clave: %s - Entrada: %d - Tamanio: %d", key, entry->id, entry->size);
}

void entry_destroy(entry_t* entry) {
	free(entry);
}

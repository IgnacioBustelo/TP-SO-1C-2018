#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>

#include "../../libs/mocks/printfer.h"
#include "../storage.h"

static int required_entries(int size) {
	if(size < storage->entry_size) {
		return 1;
	}

	int required_entries = size/storage->entry_size;

	return size % storage->entry_size == 0 ? required_entries : ++required_entries;
}

int main(int argc, char* argv[]) {
	int i, next_entry = 0, current_entry = 0;

	storage_init(16, 4);

	for(i = 1; next_entry < storage->entries && i < argc; i++) {
		int length = string_length(argv[i]) + 1;

		current_entry = next_entry;

		next_entry = current_entry + required_entries(length);

		messenger_show("INFO", "El valor '%s' de tamanio %d ocupa %d %s", argv[i], length, next_entry - current_entry, (next_entry - current_entry == 1) ? "entrada" : "entradas");

		if(next_entry < storage->entries) {
			storage_set(current_entry, argv[i], length);
		}

	}

	storage_show();

	storage_destroy();
}

#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>

#include "../../libs/mocks/printfer.h"
#include "../storage.h"

static int required_entries(int size) {
	if(size < storage->entry_size) {
		return 1;
	}

	else {
		int required = size/storage->entry_size;

		return (size % storage->entry_size == 0) ? required : ++required;
	}
}

int main(int argc, char* argv[]) {
	storage_init(16, 4);

	char* a = string_duplicate("Hello");
	char* b = string_duplicate("A");

	storage_set(1, a, string_length(a));
	storage_set(14, b, string_length(b));

	free(a);
	free(b);

	int i, next_entry = 0, entries_left = storage->entry_size;

	for(i = 1; i < argc; i++) {
		int length = string_length(argv[i]);
		int required = required_entries(length);

		if(entries_left >= required) {
			messenger_show("INFO", "El valor '%s' de tamanio %d ocupa %d %s", argv[i], length, required, (required == 1) ? "entrada" : "entradas");

			storage_set(next_entry, argv[i], length);

			next_entry += required;

			entries_left -= required;
		}

		else {
			messenger_show("INFO", "El valor '%s' de tamanio %d no entra en el storage");
		}
	}

	storage_show();

	storage_destroy();
}

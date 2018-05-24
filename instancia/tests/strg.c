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
	messenger_show("INFO", "Comienzo de la prueba de insercion en el Storage");

	storage_init(8, 2);

	char* random_value = "TEST";

	storage_set(0, random_value, string_length(random_value));

	messenger_show("INFO", "Estado inicial del Storage");

	storage_show();

	int i, next_entry = 0, entries_left = storage->entries;

	messenger_show("INFO", "Insercion de valores insertados como argumentos en el Storage");

	for(i = 1; i < argc; i++) {
		int length = string_length(argv[i]);
		int required = required_entries(length);

		if(entries_left >= required) {
			messenger_show("INFO", "El valor '%s' de tamanio %d ocupa %d entrada/s y puede insertarse en el Storage", argv[i], length, required);

			storage_set(next_entry, argv[i], length);

			next_entry += required;

			entries_left -= required;

			messenger_show("INFO", "Queda/n %d entrada/s libre/s y el proximo valor se insertara en la entrada %d", entries_left, next_entry);
		}

		else {
			messenger_show("WARNING", "El valor '%s' de tamanio %d no entra en el Storage porque ocupa %d entradas y quedan %d", argv[i], length, required, entries_left);
		}
	}

	messenger_show("INFO", "Fin de la prueba de insercion en el Storage");

	storage_show();

	storage_destroy();
}

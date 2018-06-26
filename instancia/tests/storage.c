#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>

#include "../../libs/mocks/printfer.h"
#include "../globals.h"
#include "../storage.h"

#define IS_SINGLE(SINGLE_WORD, PLURAL_WORD) entries_left == 1 ? SINGLE_WORD : PLURAL_WORD

int main(int argc, char* argv[]) {
	messenger_show("INFO", "Comienzo de la prueba de insercion y persistencia en el Storage");

	storage_init(8, 2);

	char* random_value = "TEST";

	storage_set(0, random_value, string_length(random_value));

	messenger_show("INFO", "Estado inicial del Storage");

	storage_show();

	int i, next_entry = 0, entries_left = storage->entries;

	messenger_show("INFO", "Insercion de valores insertados como argumentos en el Storage");

	for(i = 1; i < argc; i++) {
		int length = string_length(argv[i]);
		int required = storage_required_entries(length);

		messenger_show("INFO", "Insercion del valor %s'%s'%s en el Storage", COLOR_CYAN, argv[i], COLOR_RESET);

		int status = storage_set(next_entry, argv[i], length);

		if(status != STRG_SUCCESS) {
			messenger_show("ERROR", "No se puede ingresar el valor %s en la tabla de entradas", argv[i]);

			continue;
		}

		next_entry += required;

		entries_left -= required;

		if(entries_left == 0) {
			messenger_show("WARNING", "No quedan entradas libres");

			continue;
		}

		messenger_show("INFO", "%s %d %s %s", IS_SINGLE("Queda", "Quedan"), entries_left, IS_SINGLE("entrada", "entradas"), IS_SINGLE("libre", "libres"));

		messenger_show("INFO", "La proxima entrada a ocupar es la %d", next_entry);
	}

	messenger_show("INFO", "Fin de inserciones");

	messenger_show("INFO", "Estado final del Storage");

	storage_show();

	messenger_show("INFO", "Fin de la prueba de insercion en el Storage");

	storage_destroy();
}

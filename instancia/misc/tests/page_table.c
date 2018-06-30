#include <commons/string.h>

#include "../../../libs/messenger.h"
#include "../../../libs/mocks/color.h"

#include "../page_table.h"

static entry_t* page_table_entry_generator(char* value) {
	static int current_id = 0;

	size_t size = string_length(value);

	entry_t* generated_entry = entry_new(current_id, size);

	current_id += size;

	return generated_entry;
}

int main(int argc, char* argv[]) {
	page_table_init();

	if(argc == 1) {
		messenger_show("WARNING", "Ingrese pares clave y valor separados por coma para hacer la prueba");

		page_table_destroy();

		exit(EXIT_FAILURE);
	}

	int i;
	for(i = 1; i < argc; i++) {
		if(!string_contains(argv[i], ",")) {
			messenger_show("WARNING", "La cadena '%s' No esta separada por comas", argv[i]);

			continue;
		}

		char** csv = string_split(argv[i], ",");

		entry_t* entry = page_table_entry_generator(csv[1]);

		if(page_table_exists_key(csv[0])) {
			page_table_update(csv[0], entry);

			messenger_show("INFO", "Se actualizo la clave %s%s%s con valor %s%s%s", COLOR_MAGENTA, csv[0], COLOR_RESET, COLOR_GREEN, csv[1], COLOR_RESET);
		}

		else {
			page_table_insert(csv[0], entry);

			messenger_show("INFO", "Se inserto la clave %s%s%s con valor %s%s%s", COLOR_CYAN, csv[0], COLOR_RESET, COLOR_GREEN, csv[1], COLOR_RESET);
		}

		string_iterate_lines(csv, (void*) free);

		free(csv);
	}

	page_table_show();

	page_table_destroy();
}

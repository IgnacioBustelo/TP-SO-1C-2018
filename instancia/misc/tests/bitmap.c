#include <commons/string.h>

#include "../../../libs/messenger.h"
#include "../../../libs/mocks/color.h"

#include "../bitmap.h"

int main(int argc, char* argv[]) {
	bitmap_init(8);

	bitmap_insert(3, "dummy", 2);

	messenger_show("INFO", "Estado inicial del Bitmap");

	bitmap_show();

	if(argc == 1) {
		messenger_show("WARNING", "Inserte uno o mas tamaños de valores para hacer la prueba");

		bitmap_destroy();

		exit(EXIT_FAILURE);
	}

	messenger_show("INFO", "Insercion de %d claves en el Bitmap", argc - 1);

	char** keys = calloc(argc, sizeof(char*));

	int i;
	for(i = 1; i < argc; i++) {
		int key_size = atoi(argv[i]);

		keys[i - 1] = string_from_format("clave_%d", i);

		int next_index = bitmap_next_index(key_size);

		if(next_index == -1) {
			continue;
		}

		bitmap_insert(next_index, keys[i - 1], key_size);

		messenger_show("INFO", "Insercion de la clave %s%s%s en la entrada %s%d%s ocupando %d entrada/s en el Bitmap", COLOR_CYAN, keys[i - 1], COLOR_RESET, COLOR_GREEN, next_index, COLOR_RESET, key_size);
	}

	messenger_show("INFO", "Estado final del Bitmap");

	bitmap_show();

	bitmap_destroy();

	string_iterate_lines(keys, (void*) free);

	free(keys);
}

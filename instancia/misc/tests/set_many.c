#include <commons/string.h>

#include "../../../libs/messenger.h"
#include "../../../libs/mocks/color.h"

#include "../bitmap.h"

#define KEY			"test"
#define FIRST_INDEX 2
#define FIRST_SIZE	3

int main(int argc, char* argv[]) {
	int previous_index = FIRST_INDEX;
	size_t previous_size = FIRST_SIZE;

	bitmap_init(16);

	bitmap_insert(FIRST_INDEX, KEY, FIRST_SIZE);

	bitmap_insert(7, "dummy", 2);

	messenger_show("INFO", "Estado inicial del Bitmap");

	bitmap_show();

	if(argc == 1) {
		messenger_show("WARNING", "Inserte uno o mas tamanios de valores para hacer la prueba");

		bitmap_destroy();

		exit(EXIT_FAILURE);
	}

	messenger_show("INFO", "Insercion de %d valor/es sobre la clave %s'%s'%s en el Bitmap", argc - 1, COLOR_RED, KEY, COLOR_RESET);

	int i;
	for(i = 1; i < argc; i++) {
		messenger_show("INFO", "La clave a probar esta actualmente en la entrada %d y ocupa %d entrada/s", previous_index, previous_size);

		int key_size = atoi(argv[i]);

		int updated_index = bitmap_update(previous_index, KEY, key_size, previous_size);

		if(updated_index == -1) {
			messenger_show("WARNING", "No fue posible insertar un valor de tamanio %d", key_size);

			continue;
		}

		messenger_show("INFO", "Insercion de un valor de tamanio %s%d%s en la entrada %s%d%s del Bitmap", COLOR_CYAN, key_size, COLOR_RESET, COLOR_GREEN, updated_index, COLOR_RESET);

		previous_index = updated_index;

		previous_size = key_size;

		bitmap_show();
	}

	bitmap_destroy();
}

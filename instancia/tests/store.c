#include <commons/string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../libs/mocks/printfer.h"
#include "../dumper.h"

int main(int argc, char* argv[]) {
	int i, max_keys = messenger_longest_string_length(argc - 1);

	if(argc == 1) {
		messenger_show("ERROR", "Ingrese valores por la linea de comandos para persistir");

		exit(EXIT_FAILURE);
	}

	dumper_init("files/");

	messenger_show("INFO", "Se inicio el Dumper del Storage en el punto de montaje %s", dumper->mount_point);

	messenger_show("INFO", "Prueba para insertar valores de claves en el punto de montaje");

	for(i = 1; i < argc; i++) {
		char* key = string_from_format("archivo_%0.*d", max_keys, i);

		int length = string_length(argv[i]);

		dumper_store(key, argv[i], length);

		messenger_show("INFO", "Insercion del valor %s%s%s en el path %s%s, ocupando %d byte/s", COLOR_CYAN, argv[i], COLOR_RESET, dumper->mount_point, key, length);

		free(key);
	}

	messenger_show("INFO", "Se persistieron todas las claves");

	dumper_show();

	messenger_show("INFO", "Borrado de claves pares del punto de montaje");

	for(i = 1; i < argc; i++) {
		char* key = string_from_format("archivo_%0.*d", max_keys, i);

		if(i % 2 == 0) {
			dumper_remove_key_value(key);

			messenger_show("INFO", "Eliminacion de la clave %s%s%s del punto de montaje", COLOR_RED, key, COLOR_RESET);
		}

		free(key);
	}

	messenger_show("INFO", "Se elminaron las claves pares del punto de montaje");

	dumper_show();

	dumper_destroy();
}

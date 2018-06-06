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

	for(i = 1; i < argc; i++) {
		char* key = string_from_format("clave_%0.*d", max_keys, i);

		int length = string_length(argv[i]);

		dumper_store(key, argv[i], length);

		messenger_show("INFO", "Insercion del valor %s en el path %s%s, ocupando %d byte/s", argv[i], dumper->mount_point, key, length);

		free(key);
	}

	messenger_show("INFO", "Se persistieron todas las claves");

	dumper_show();

	dumper_destroy();
}

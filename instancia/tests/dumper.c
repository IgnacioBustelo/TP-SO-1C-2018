#include <commons/string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../../libs/mocks/printfer.h"
#include "../dumper.h"

int main(int argc, char* argv[]) {
	int i, max_keys = messenger_longest_string_length(argc);

	if(argc == 1) {
		messenger_show("ERROR", "Ingrese valores por la linea de comandos para persistir");

		exit(EXIT_FAILURE);
	}

	dumper_init("files/");

	messenger_show("INFO", "Se inicio el Dumper del Storage en el punto de montaje %s", dumper_mount_point);

	for(i = 1; i < argc; i++) {
		char* key = string_from_format("clave_%*.d", max_keys, i);

		int fd_key = dumper_create_key_value(key);

		dumper_store(fd_key, argv[i], string_length(argv[i]));

		messenger_show("INFO", "Se inicio el Dumper del Storage en el punto de montaje %s", dumper_mount_point);

		close(fd_key);

		free(key);
	}

	messenger_show("INFO", "Se persistieron todas las claves");
}

#include <commons/string.h>
#include <stdlib.h>

#include "instancia.h"

#define PATH "configs"

int main(int argc, char* argv[]) {
	char* cfg_path;

	cfg_path = (argc < 2) ? string_duplicate("instancia.cfg") : string_from_format("%s/%s.cfg", PATH, argv[1]);

	int status = instance_init("Instancia", "instancia.log", (argc < 3) ? "INFO" : argv[2], cfg_path);

	if(status == INSTANCE_INIT_SUCCESS) {
		instance_main();

		instance_die();
	}

	free(cfg_path);
}

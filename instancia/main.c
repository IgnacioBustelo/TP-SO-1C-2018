#include "instancia.h"

int main(int argc, char* argv[]) {
	int status = instance_init("Instancia", "instancia.log", (argc == 1) ? "INFO" : argv[1], "instancia.cfg");

	if(status == INSTANCE_INIT_ERROR) {
		exit(EXIT_FAILURE);
	}

	instance_main();

	instance_die();
}

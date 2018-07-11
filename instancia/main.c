#include "instancia.h"

int main(int argc, char* argv[]) {
	instance_init("Instancia", "instancia.log", (argc == 1) ? "INFO" : argv[1], "instancia.cfg");

	instance_main();

	instance_die();
}

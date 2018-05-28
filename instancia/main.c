#include "bootstrap.h"
#include "instancia.h"

int main(int argc, char* argv[]) {
	bootstrap_start("Instancia", "instancia.log", (argc == 1) ? "INFO" : argv[1], "instancia.cfg");

	instance_main();

	instance_die();
}

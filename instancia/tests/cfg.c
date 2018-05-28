#include <commons/string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../../libs/mocks/printfer.h"
#include "../cfg_instancia.h"

int main() {
	cfg_instancia_init("../instancia.cfg");

	cfg_instancia_show();

	cfg_instancia_destroy();
}

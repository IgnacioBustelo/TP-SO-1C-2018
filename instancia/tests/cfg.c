#include <commons/string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../cfg_instancia.h"

void cfg_show(const char* format,...) {
	va_list args;

	va_start(args, format);

	char* message = string_from_vformat(format, args);

	messenger_log(message, "INFO");

	va_end(args);

	free(message);
}

int main() {
	cfg_instancia_init("../instancia.cfg");

	cfg_show("Se muestran los contenidos accediendo a los valores con la interfaz de cfg_instancia");

	cfg_show("La IP del Coordinador es %s", cfg_instancia_get_coordinador_ip());
	cfg_show("El puerto Coordinador es %d", cfg_instancia_get_coordinador_port());
	cfg_show("El algoritmo de reemplazo es %s", cfg_instancia_get_replacement_algorithm_name());
	cfg_show("El punto de montaje es %s", cfg_instancia_get_mount_point());
	cfg_show("El nombre de la instancia es %s", cfg_instancia_get_instance_name());
	cfg_show("El intervalo de dump es %d", cfg_instancia_get_dump_time());

	cfg_instancia_destroy();
}

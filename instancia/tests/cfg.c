#include <commons/string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "../../libs/mocks/printfer.h"
#include "../cfg_instancia.h"

int main() {
	cfg_instancia_init("../instancia.cfg");

	messenger_show("INFO", "Se muestran los contenidos accediendo a los valores con la interfaz de cfg_instancia");

	messenger_show("INFO", "La IP del Coordinador es %s", cfg_instancia_get_coordinador_ip());
	messenger_show("INFO", "El puerto Coordinador es %d", cfg_instancia_get_coordinador_port());
	messenger_show("INFO", "El algoritmo de reemplazo es %s", cfg_instancia_get_replacement_algorithm_name());
	messenger_show("INFO", "El punto de montaje es %s", cfg_instancia_get_mount_point());
	messenger_show("INFO", "El nombre de la instancia es %s", cfg_instancia_get_instance_name());
	messenger_show("INFO", "El intervalo de dump es %d", cfg_instancia_get_dump_time());

	cfg_instancia_destroy();
}

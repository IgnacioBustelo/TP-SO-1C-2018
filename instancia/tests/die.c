#include <unistd.h>

#include "../../libs/messenger.h"
#include "../../libs/mocks/color.h"
#include "../../libs/mocks/printfer.h"
#include "../event_handler.h"

char* color_get() {
	return instance_is_alive ? COLOR_RESET : COLOR_RED;
}

int main(int argc, char* argv[]) {
	int time = (argc < 2) ? 1 : atoi(argv[1]), time_passed = 0;

	event_handler_init();

	messenger_show("INFO", "Prueba de desconexion de una Instancia");

	messenger_show("INFO", "Pulse CTRL+C para desconectar la Instancia");

	do {

		alarm(time);

		pause();

		if(instance_requires_dump && instance_is_alive) {
			time_passed += time;

			messenger_show("INFO", "Han pasado %d segundo/s desde el comienzo de la prueba", time_passed);

			instance_requires_dump = false;
		}

	} while (instance_is_alive);

	printfer_next_line();

	messenger_show("INFO", "La Instancia fue asesinada luego de %d segundos. QEPD...", time_passed);
}

#include "../../libs/messenger.h"
#include "../../libs/mocks/color.h"
#include "../../libs/mocks/printfer.h"
#include "../terminator.h"

char* color_get() {
	return terminator_is_alive() ? COLOR_RESET : COLOR_RED;
}

int main() {
	terminator_init();

	messenger_show("INFO", "Prueba de desconexion de una Instancia");

	messenger_show("INFO", "Pulse CTRL+C para desconectar la Instancia");

	while(terminator_is_alive());

	printfer_next_line();

	messenger_show("INFO", "La Instancia fue asesinada. QEPD...");
}

#include <commons/string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../messenger.h"
#include "../mocks/color.h"
#include "../mocks/printfer.h"

int parity;

char* color_get() {
	return parity % 2 == 0 ? (parity == 0) ? "" : COLOR_RED : COLOR_CYAN;
}

int main(int argc, char* argv[]) {
	parity = 0;

	messenger_show("INFO", "Prueba de PRINTFER para imprimir los argumentos del programa por pantalla");

	int i;
	for(i = 1; i < argc; i++) {
		parity = i;

		messenger_show("INFO", "Argumento %d: %s", i, argv[i]);
	}

}

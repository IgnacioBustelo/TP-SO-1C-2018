#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../libs/messenger.h"
#include "event_handler.h"

void(*destroyer)();

void event_handler_init() {
	instance_is_alive = true;

	struct sigaction action;

	memset(&action, 0, sizeof(struct sigaction));

	sigemptyset(&action.sa_mask);

	action.sa_handler = event_handler_action;

	action.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &action, NULL);

	sigaction(SIGTERM, &action, NULL);

	// TODO: Manejar errores
}

void event_handler_init_with_finisher(void(*finisher)()) {
	event_handler_init();

	destroyer = finisher;
}

void event_handler_action(int signal) {
	if(signal == SIGINT || signal == SIGTERM) {
		messenger_show("WARNING", "Se envio la señal %s para desconectar a la Instancia", (signal == SIGINT) ? "SIGINT" : "SIGTERM");

		instance_is_alive = false;

		destroyer();

		exit(EXIT_SUCCESS);
	}
}

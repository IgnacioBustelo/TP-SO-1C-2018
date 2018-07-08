#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../libs/messenger.h"
#include "event_handler.h"

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

void event_handler_action(int signal) {
	if(signal == SIGINT || signal == SIGTERM) {
		instance_is_alive = false;
	}
}

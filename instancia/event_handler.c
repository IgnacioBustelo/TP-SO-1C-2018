#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../libs/messenger.h"
#include "event_handler.h"

void event_handler_init() {
	instance_is_alive = true, instance_requires_dump = false;

	struct sigaction action;

	memset(&action, 0, sizeof(struct sigaction));

	sigemptyset(&action.sa_mask);

	action.sa_handler = event_handler_action;

	action.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &action, NULL);

	sigaction(SIGTERM, &action, NULL);

	sigaction(SIGALRM, &action, NULL);

	// TODO: Manejar errores
}

void event_handler_action(int signal) {
	switch(signal) {
		case SIGINT		: instance_is_alive = false;		break;

		case SIGTERM	: instance_is_alive = false; 		break;

		case SIGALRM	: instance_requires_dump = true;	break;

		default			: /* TODO: No se... */				break;
	}
}

void event_handler_alarm(int time) {
	alarm(time);
}

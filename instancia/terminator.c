#include <signal.h>
#include <string.h>

#include "../libs/messenger.h"
#include "terminator.h"

bool alive;

void terminator_init() {
	alive = true;

	struct sigaction action;

	memset(&action, 0, sizeof(struct sigaction));

	sigemptyset(&action.sa_mask);

	action.sa_sigaction = terminator_die;

	action.sa_flags = SA_SIGINFO;

	sigaction(SIGINT, &action, NULL);

	// TODO: Manejar errores
}

void terminator_die() {
	alive = false;
}

bool terminator_is_alive() {
	return alive;
}

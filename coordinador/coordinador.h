#ifndef COORDINADOR_COORDINADOR_H_
#define COORDINADOR_COORDINADOR_H_

#include <commons/log.h>

enum distribution_algorithm_t {
	LSU, EL, KE
};

struct setup_t {
	int port;
	enum distribution_algorithm_t distribution;
	int entries_num;
	int entries_size;
	int delay;
};

/* Shared Global Variables */
extern t_log *logger;
extern struct setup_t setup;

void exit_gracefully(int status);

#endif

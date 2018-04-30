#ifndef COORDINADOR_CONFIG_H_
#define COORDINADOR_CONFIG_H_

#include <stdbool.h>

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

extern struct setup_t setup;

void init_config();

#endif /* COORDINADOR_CONFIG_H_ */

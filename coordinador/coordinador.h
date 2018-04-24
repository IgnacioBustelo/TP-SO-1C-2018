#ifndef COORDINADOR_COORDINADOR_H_
#define COORDINADOR_COORDINADOR_H_

#include <commons/log.h>

#define synchronized(lock)												\
	for (pthread_mutex_t * i_##lock = &lock; i_##lock;					\
		i_##lock = NULL, pthread_mutex_unlock(&lock))					\
		for (pthread_mutex_lock(i_##lock); i_##lock; i_##lock = NULL)

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

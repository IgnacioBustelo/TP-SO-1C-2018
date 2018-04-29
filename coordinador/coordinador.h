#ifndef COORDINADOR_COORDINADOR_H_
#define COORDINADOR_COORDINADOR_H_

#include <commons/log.h>

#define synchronized(lock)												\
	for (pthread_mutex_t * i_##lock = &lock; i_##lock;					\
		i_##lock = NULL, pthread_mutex_unlock(&lock))					\
		for (pthread_mutex_lock(i_##lock); i_##lock; i_##lock = NULL)

void exit_gracefully(int status);

#endif

#include <stdbool.h>
#include <stddef.h>

#include "distribution.h"
#include "instance-list.h"

#define synchronized(lock)									\
	for (pthread_mutex_t * i_ = &lock; i_;					\
		i_ = NULL, pthread_mutex_unlock(&lock))				\
		for (pthread_mutex_lock(i_); i_; i_ = NULL)

struct instance_t *equitative_load(struct instance_list_t *instance_list)
{
	struct instance_t *next_instance;
	synchronized(instance_list->lock) {
		next_instance = instance_list_pop(instance_list);
		instance_list_push(instance_list, next_instance);
	}

	return next_instance;
}

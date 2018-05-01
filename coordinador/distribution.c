#include <stdbool.h>
#include <stddef.h>

#include "defines.h"
#include "instance-list/instance-list.h"

#include "distribution.h"

struct instance_t *equitative_load(struct instance_list_t *instance_list)
{
	struct instance_t *next_instance;
	synchronized(instance_list->lock) {
		next_instance = instance_list_pop(instance_list);
		if (next_instance != NULL) {
			instance_list_push(instance_list, next_instance);
		}
	}

	return next_instance;
}

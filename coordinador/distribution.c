#include <stdbool.h>
#include <stddef.h>

#include "defines.h"
#include "instance-list/instance-list.h"

#include "key-table/key-table.h"
#include "distribution.h"

struct instance_t *equitative_load(struct instance_list_t *instance_list, char *key)
{
	struct instance_t *instance = key_table_get_instance(key);
	if (instance != NULL) {
		return instance;
	}

	struct instance_t *next_instance;
	synchronized(instance_list->lock) {
		next_instance = instance_list_pop(instance_list);
		if (next_instance != NULL) {
			instance_list_push(instance_list, next_instance);
		}
	}

	return next_instance;
}

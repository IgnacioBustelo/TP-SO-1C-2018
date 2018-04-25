#include <stdbool.h>
#include <stddef.h>

#include "distribution.h"
#include "instance-list.h"

t_list *last_instances;

static void last_instances_add(struct instance_t *elem, t_list *instance_list);
static bool list_contains(t_list *list, void *member);

/* Executed before main() */
__attribute__((constructor)) void init(void) {
	last_instances = list_create();
}

/* Executed after main() */
__attribute__((destructor)) void destroy(void) {
	list_destroy(last_instances);
}

struct instance_t *equitative_load(t_list *instance_list)
{
	if (list_size(instance_list) >= list_size(last_instances)) {
		list_remove(last_instances, 0);
	}

	bool last_instances_not_contains(void *elem) {
		return !list_contains(last_instances, elem);
	}

	void *_next_instance = list_find(instance_list, last_instances_not_contains);
	struct instance_t *next_instance = (struct instance_t *)_next_instance;
	if (next_instance != NULL) {
		last_instances_add(next_instance, instance_list);
	}

	return next_instance;
}

static void last_instances_add(struct instance_t *elem, t_list *instance_list)
{
	/* TODO: Mutexes */
	list_add(last_instances, elem);
	while (list_size(last_instances) >= list_size(instance_list)) {
		list_remove(last_instances, 0);
	}
}

static bool list_contains(t_list *list, void *member)
{
	bool equals(void *elem) {
		return member == elem;
	}
	return list_any_satisfy(list, equals);
}

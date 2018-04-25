#include <stdlib.h>
#include <stddef.h>
#include <semaphore.h>

#include <commons/collections/list.h>

#include "instance-list.h"

/* TODO:
 *   - Mutexes
 *   - Request structure
 *   - Request list functions
 */

static struct instance_t *instance_list_node_create(int fd);
static void instance_list_node_destroy(struct instance_t *victim);

t_list *instance_list_create(void)
{
	return list_create();
}

struct instance_t *instance_list_add(t_list *instance_list, int fd)
{
	struct instance_t *node = instance_list_node_create(fd);
	list_add(instance_list, node);
	return node;
}

struct instance_t* instance_list_remove(t_list* instance_list, int fd)
{
	bool find_by_fd(void *elem) {
		struct instance_t *node = elem;
		return node->fd == fd;
	}
	return (struct instance_t*)list_remove_by_condition(instance_list, find_by_fd);
}

bool instance_list_delete(t_list *instance_list, int fd)
{
	struct instance_t *victim = instance_list_remove(instance_list, fd);

	if (victim != NULL) {
		instance_list_node_destroy(victim);
		return true;
	} else {
		return false;
	}
}

static struct instance_t *instance_list_node_create(int fd)
{
	struct instance_t *node = malloc(sizeof(*node));
	node->fd = fd;
	node->requests = list_create();
	sem_init(&node->requests_count, 0, 0);

	return node;
}

static void instance_list_node_destroy(struct instance_t *victim)
{
	/* TODO:
	 * request_list_destroy(victim->requests);
	 */
	sem_destroy(&victim->requests_count);
	free(victim);
}

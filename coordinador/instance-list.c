#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include <commons/collections/list.h>

#include "instance-list.h"

/* TODO:
 *   - Request structure
 *   - Request list functions
 */

static struct instance_t *instance_list_node_create(char *name, int fd);
static void instance_list_node_destroy(struct instance_t *victim);
static bool string_equals(char *actual, char *expected);

struct instance_list_t *instance_list_create(void)
{
	struct instance_list_t *instance_list = malloc(sizeof(*instance_list));
	pthread_mutex_init(&instance_list->lock, NULL);
	instance_list->elements = list_create();
	return instance_list;
}

struct instance_t *instance_list_add(struct instance_list_t *instance_list, char *name, int fd)
{
	struct instance_t *node = instance_list_node_create(name, fd);
	list_add_in_index(instance_list->elements, 0, node);
	return node;
}

struct instance_t *instance_list_remove(struct instance_list_t *instance_list, char *name)
{
	bool find_by_name(void *elem) {
		struct instance_t *node = elem;
		return string_equals(node->name, name);
	}
	return (struct instance_t *)list_remove_by_condition(instance_list->elements, find_by_name);
}

struct instance_t *instance_list_push(struct instance_list_t *instance_list, struct instance_t *elem)
{
	list_add(instance_list->elements, elem);
	return elem;
}

struct instance_t *instance_list_pop(struct instance_list_t *instance_list)
{
	return list_remove(instance_list->elements, 0);
}

bool instance_list_delete(struct instance_list_t *instance_list, char *name)
{
	struct instance_t *victim = instance_list_remove(instance_list, name);

	if (victim != NULL) {
		instance_list_node_destroy(victim);
		return true;
	} else {
		return false;
	}
}

static struct instance_t *instance_list_node_create(char *name, int fd)
{
	struct instance_t *node = malloc(sizeof(*node));
	node->name = name;
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
	free(victim->name);
	sem_destroy(&victim->requests_count);
	free(victim);
}

static bool string_equals(char *actual, char *expected)
{
	return strcmp(actual, expected) == 0;
}

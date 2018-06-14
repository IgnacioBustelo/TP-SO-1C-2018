#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include <commons/collections/list.h>

#include "instance-list.h"

#include "instance-request-list.h"

static struct instance_t *instance_list_node_create(char *name, int fd);
static void instance_list_node_destroy(struct instance_t *victim);
static bool string_equals(char *actual, char *expected);
static bool instance_is_connected(void *_instance);

/* TODO:
 *   - Checkear si la instancia tiene espacio disponible.
 */

struct instance_list_t *instance_list_create(void)
{
	struct instance_list_t *instance_list = malloc(sizeof(*instance_list));
	pthread_mutex_init(&instance_list->lock, NULL);
	instance_list->elements = list_create();
	return instance_list;
}

void instance_list_destroy(struct instance_list_t *victim)
{
	void destroyer(void *elem) {
		instance_list_node_destroy((struct instance_t *)elem);
	}

	pthread_mutex_destroy(&victim->lock);
	list_destroy_and_destroy_elements(victim->elements, destroyer);
	free(victim);
}

size_t instance_list_size(struct instance_list_t *instance_list)
{
	return list_count_satisfying(instance_list->elements, instance_is_connected);
}

struct instance_t *instance_list_get_by_name(struct instance_list_t *instance_list, char *name)
{
	bool find_by_name(void *elem) {
		struct instance_t *node = elem;
		return string_equals(node->name, name);
	}

	return (struct instance_t *)list_find(instance_list->elements, find_by_name);
}

struct instance_t *instance_list_get_by_index(struct instance_list_t *instance_list, int index)
{
	int i = 0;
	bool is_connected_instance_with_index(void *instance) {
		if (instance_is_connected(instance)) {
			if (i == index) {
				return true;
			} else {
				i += 1;
			}
		}
		return false;
	}

	return (struct instance_t *)list_find(instance_list->elements, is_connected_instance_with_index);
}

struct instance_t *instance_list_add(struct instance_list_t *instance_list, char *name, int fd)
{
	struct instance_t *node = instance_list_get_by_name(instance_list, name);
	if (node == NULL) {
		node = instance_list_node_create(name, fd);
		list_add_in_index(instance_list->elements, 0, node);
	} else if (node->fd == DISCONNECTED) {
		node->fd = fd;
	} else {
		return NULL;
	}

	return node;
}

struct instance_t *instance_list_remove(struct instance_list_t *instance_list, char *name)
{
	struct instance_t *node = instance_list_get_by_name(instance_list, name);
	if (node != NULL) {
		node->fd = DISCONNECTED;
	}

	return node;
}

struct instance_t *instance_list_first(struct instance_list_t *instance_list)
{
	return list_find(instance_list->elements, instance_is_connected);
}

struct instance_t *instance_list_push(struct instance_list_t *instance_list, struct instance_t *elem)
{
	list_add(instance_list->elements, elem);
	return elem;
}

struct instance_t *instance_list_pop(struct instance_list_t *instance_list)
{
	return list_remove_by_condition(instance_list->elements, instance_is_connected);
}

void instance_list_sort(struct instance_list_t *instance_list, bool (*comparator)(void *, void *))
{
	list_sort(instance_list->elements, comparator);
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
	node->name = strdup(name);
	node->fd = fd;
	node->used_entries = 0;
	node->requests = request_list_create();

	return node;
}

static void instance_list_node_destroy(struct instance_t *victim)
{
	free(victim->name);
	request_list_destroy(victim->requests);
	free(victim);
}

static bool string_equals(char *actual, char *expected)
{
	return strcmp(actual, expected) == 0;
}

static bool instance_is_connected(void *_instance)
{
	struct instance_t *instance = (struct instance_t *)_instance;
	return instance->fd != DISCONNECTED;
}

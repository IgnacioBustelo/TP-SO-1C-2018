#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>

#include <commons/collections/list.h>

#include "../defines.h"
#include "../../protocolo/protocolo_coordinador_instancia.h"
#include "instance-list.h"

#include "instance-request-list.h"

static struct instance_t *instance_list_node_create(char *name, int fd);
static void instance_list_node_destroy(struct instance_t *victim);
static bool string_equals(char *actual, char *expected);
static bool instance_is_connected(void *_instance);
static bool instance_test_connection(struct instance_t *instance);
static bool list_contains(t_list *list, void *elem);

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

size_t instance_list_size(struct instance_list_t *instance_list, t_list *excluded_instances)
{
	bool instance_is_available(void *instance) {
		return (excluded_instances == NULL || !list_contains(excluded_instances, instance))
				&& instance_is_connected(instance);
	}

	return list_count_satisfying(instance_list->elements, instance_is_available);
}

struct instance_t *instance_list_get_by_name(struct instance_list_t *instance_list, char *name)
{
	bool find_by_name(void *elem) {
		struct instance_t *node = elem;
		return string_equals(node->name, name);
	}

	return (struct instance_t *)list_find(instance_list->elements, find_by_name);
}

struct instance_t *instance_list_get_by_index(struct instance_list_t *instance_list, int index, t_list *excluded_instances)
{
	bool instance_is_available(void *instance) {
		return (excluded_instances == NULL || !list_contains(excluded_instances, instance))
				&& instance_is_connected(instance);
	}

	int i = 0;
	bool is_available_instance_with_index(void *instance) {
		if (instance_is_available(instance)) {
			if (i == index) {
				return true;
			} else {
				i += 1;
			}
		}
		return false;
	}

	return (struct instance_t *)list_find(instance_list->elements, is_available_instance_with_index);
}

struct instance_t *instance_list_add(struct instance_list_t *instance_list, char *name, int fd)
{
	struct instance_t *instance = instance_list_get_by_name(instance_list, name);
	if (instance == NULL) {
		instance = instance_list_node_create(name, fd);
		list_add_in_index(instance_list->elements, 0, instance);
	} else if (instance->fd == DISCONNECTED || !instance_test_connection(instance)) {
		synchronized(instance->lock) {
			instance->fd = fd;
		}
	} else {
		return NULL;
	}

	return instance;
}

struct instance_t *instance_list_remove(struct instance_list_t *instance_list, char *name)
{
	struct instance_t *node = instance_list_get_by_name(instance_list, name);
	if (node != NULL) {
		node->fd = DISCONNECTED;
	}

	return node;
}

struct instance_t *instance_list_first(struct instance_list_t *instance_list, t_list *excluded_instances)
{
	bool instance_is_available(void *instance) {
		return (excluded_instances == NULL || !list_contains(excluded_instances, instance))
				&& instance_is_connected(instance);
	}
	return list_find(instance_list->elements, instance_is_available);
}

struct instance_t *instance_list_push(struct instance_list_t *instance_list, struct instance_t *elem)
{
	list_add(instance_list->elements, elem);
	return elem;
}

struct instance_t *instance_list_pop(struct instance_list_t *instance_list, t_list *excluded_instances)
{
	bool instance_is_available(void *instance) {
		return (excluded_instances == NULL || !list_contains(excluded_instances, instance))
				&& instance_is_connected(instance);
	}
	return list_remove_by_condition(instance_list->elements, instance_is_available);
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

void instance_list_iterate(struct instance_list_t *instance_list, void (*closure)(void *))
{
	t_list *connected_instance_list = list_filter(instance_list->elements, instance_is_connected);
	list_iterate(connected_instance_list, closure);
}

static struct instance_t *instance_list_node_create(char *name, int fd)
{
	struct instance_t *node = malloc(sizeof(*node));
	pthread_mutex_init(&node->lock, NULL);
	node->name = strdup(name);
	node->fd = fd;
	node->used_entries = 0;
	node->requests = request_list_create();

	return node;
}

static void instance_list_node_destroy(struct instance_t *victim)
{
	pthread_mutex_destroy(&victim->lock);
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

static bool instance_test_connection(struct instance_t *instance)
{
	pthread_mutex_lock(&instance->lock);

	request_coordinador request = PROTOCOL_CI_IS_ALIVE;
	if (!CHECK_SEND(instance->fd, &request)) {
		pthread_mutex_unlock(&instance->lock);
		return false;
	}

	request_instancia response;
	if (!CHECK_RECV(instance->fd, &response) || response != PROTOCOL_IC_CONFIRM_CONNECTION) {
		pthread_mutex_unlock(&instance->lock);
		return false;
	}

	pthread_mutex_unlock(&instance->lock);
	return true;
}

static bool list_contains(t_list *list, void *elem)
{
	bool equals(void *find_list_elem) {
		return find_list_elem == elem;
	}

	return list_find(list, equals) != NULL;
}

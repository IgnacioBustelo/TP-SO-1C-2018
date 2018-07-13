#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include <commons/collections/queue.h>

#include "../defines.h"
#include "instance-request-list.h"

static struct request_node_t *request_node_create_set(int esi_fd, char *key, char *value);
static struct request_node_t *request_node_create_store(int esi_fd, char *key);

struct request_list_t *request_list_create(void)
{
	struct request_list_t *list = malloc(sizeof(*list));
	pthread_mutex_init(&list->lock, NULL);
	sem_init(&list->count, 0, 0);
	list->elements = queue_create();

	return list;
}

void request_list_destroy(struct request_list_t *list)
{
	void destructor(void *elem) {
		request_node_destroy((struct request_node_t *)elem);
	}
	pthread_mutex_destroy(&list->lock);
	queue_destroy_and_destroy_elements(list->elements, destructor);
	sem_destroy(&list->count);
	free(list);
}

struct request_node_t *request_list_push_set(struct request_list_t *request_list, int esi_fd, char *key, char *value)
{
	struct request_node_t *node = request_node_create_set(esi_fd, key, value);
	request_list_push(request_list, node);

	return node;
}

struct request_node_t *request_list_push_store(struct request_list_t *request_list, int esi_fd, char *key)
{
	struct request_node_t *node = request_node_create_store(esi_fd, key);
	request_list_push(request_list, node);

	return node;
}

void request_list_push(struct request_list_t *request_list, struct request_node_t *elem)
{
	synchronized(request_list->lock) {
		queue_push(request_list->elements, elem);
	}
	sem_post(&request_list->count);
}

struct request_node_t *request_list_pop(struct request_list_t *request_list)
{
	struct request_node_t *node;
	sem_wait(&request_list->count);
	synchronized(request_list->lock) {
		node = (struct request_node_t *)queue_pop(request_list->elements);
	}

	return node;
}

void request_list_iterate(struct request_list_t *request_list, void (*closure)(void *))
{
	list_iterate(request_list->elements->elements, closure);
}

static struct request_node_t *request_node_create_set(int esi_fd, char *key, char *value)
{
	struct request_node_t *node = malloc(sizeof(*node));
	node->requesting_esi_fd = esi_fd;
	node->type = INSTANCE_SET;
	node->set.key = strdup(key);
	node->set.value = strdup(value);
	node->excluded_instances = list_create();

	return node;
}

static struct request_node_t *request_node_create_store(int esi_fd, char *key)
{
	struct request_node_t *node = malloc(sizeof(*node));
	node->requesting_esi_fd = esi_fd;
	node->type = INSTANCE_STORE;
	node->store.key = strdup(key);
	node->excluded_instances = list_create();

	return node;
}

void request_node_destroy(struct request_node_t *node)
{
	switch (node->type) {
	case INSTANCE_SET:
		free(node->set.key);
		free(node->set.value);
		break;
	case INSTANCE_STORE:
		free(node->store.key);
		break;
	}

	list_destroy(node->excluded_instances);
	free(node);
}

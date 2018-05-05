#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#include <commons/collections/queue.h>

#include "../defines.h"
#include "instance-request-list.h"

static struct request_node_t *request_node_create_set(char *key, char *value);
static struct request_node_t *request_node_create_store(char *key);
static void request_list_push(struct request_list_t *request_list, struct request_node_t *elem);
static void request_node_destroy(struct request_node_t *node);

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
}

struct request_node_t *request_list_push_set(struct request_list_t *request_list, char *key, char *value)
{
	struct request_node_t *node = request_node_create_set(key, value);
	request_list_push(request_list, node);

	return node;
}

struct request_node_t *request_list_push_store(struct request_list_t *request_list, char *key)
{
	struct request_node_t *node = request_node_create_store(key);
	request_list_push(request_list, node);

	return node;
}

static void request_list_push(struct request_list_t *request_list, struct request_node_t *elem)
{
	synchronized(request_list->lock) {
		queue_push(request_list->elements, elem);
		sem_post(&request_list->count);
	}
}

struct request_node_t *request_list_pop(struct request_list_t *request_list)
{
	struct request_node_t *node;
	synchronized(request_list->lock) {
		sem_wait(&request_list->count);
		node = (struct request_node_t *)queue_pop(request_list->elements);
	}

	return node;
}

static struct request_node_t *request_node_create_set(char *key, char *value)
{
	struct request_node_t *node = malloc(sizeof(*node));
	node->type = INSTANCE_SET;
	node->set.key = strdup(key);
	node->set.value = strdup(value);

	return node;
}

static struct request_node_t *request_node_create_store(char *key)
{
	struct request_node_t *node = malloc(sizeof(*node));
	node->type = INSTANCE_STORE;
	node->store.key = strdup(key);

	return node;
}

static void request_node_destroy(struct request_node_t *node)
{
	switch (node->type) {
	case INSTANCE_SET:
		free(node->set.key);
		free(node->set.value);
		break;
	case INSTANCE_STORE:
		free(node->store.key);
	}

	free(node);
}

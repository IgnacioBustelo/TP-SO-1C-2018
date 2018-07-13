#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include <commons/collections/list.h>

#include "defines.h"
#include "instance-list/instance-list.h"

#include "key-table/key-table.h"
#include "distribution.h"
#include "config.h"

struct setup_t setup;

static struct instance_t *equitative_load(struct instance_list_t *instance_list, char *key, t_list *excluded_instances);

static bool sort_by_used_space(void *a, void *b);
static struct instance_t *least_space_used(struct instance_list_t *instance_list, char *key, t_list *excluded_instances);

static struct instance_t *key_explicit(struct instance_list_t *instance_list, char *key, t_list *excluded_instances);

struct instance_t *dispatch(struct instance_list_t *instance_list, char *key, t_list *excluded_instances)
{
	struct instance_t *instance = key_table_get_instance(key);
	if (instance != NULL) {
		if (instance->fd != DISCONNECTED) {
			return instance;
		} else {
			key_table_remove(key);
		}
	}

	switch (setup.distribution) {
	case EL:
		return equitative_load(instance_list, key, excluded_instances);
	case LSU:
		return least_space_used(instance_list, key, excluded_instances);
	case KE:
		return key_explicit(instance_list, key, excluded_instances);
	}

	return NULL;
}

static struct instance_t *equitative_load(struct instance_list_t *instance_list, char *key, t_list *excluded_instances)
{
	struct instance_t *next_instance;
	synchronized(instance_list->lock) {
		next_instance = instance_list_pop(instance_list, excluded_instances);
		if (next_instance != NULL) {
			instance_list_push(instance_list, next_instance);
			key_table_create_key(key, next_instance);
		}
	}

	return next_instance;
}

static bool sort_by_used_space(void *a, void *b)
{
	struct instance_t *instance_a = (struct instance_t *)a;
	struct instance_t *instance_b = (struct instance_t *)b;

	return instance_a->used_entries <= instance_b->used_entries;
}

static struct instance_t *least_space_used(struct instance_list_t *instance_list, char *key, t_list *excluded_instances)
{
	struct instance_t *next_instance;
	synchronized(instance_list->lock) {
		instance_list_sort(instance_list, sort_by_used_space);
		next_instance = instance_list_first(instance_list, excluded_instances);
	}
	if (next_instance != NULL) {
		key_table_create_key(key, next_instance);
	}

	return next_instance;
}

static struct instance_t *key_explicit(struct instance_list_t *instance_list, char *key, t_list *excluded_instances)
{
	char first_char = tolower(key[0]);

	struct instance_t *instance;

	synchronized(instance_list->lock) {
		int index = (first_char - 'a') * instance_list_size(instance_list, excluded_instances) / ('z' - 'a' + 1);
		instance = instance_list_get_by_index(instance_list, index, excluded_instances);
	}
	if (instance != NULL) {
		key_table_create_key(key, instance);
	}

	return instance;
}

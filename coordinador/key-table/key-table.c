#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <commons/collections/dictionary.h>

#include "../defines.h"
#include "key-table.h"

struct key_table_data_t {
	bool initialized;
	struct instance_t *instance;
};

struct key_table_t {
	t_dictionary *table;
	pthread_mutex_t lock;
};

static struct key_table_t *key_table;

static struct key_table_data_t *key_table_get(char *key);
static struct key_table_data_t *key_table_data_create(struct instance_t *instance);

__attribute__((constructor)) void init_key_table(void) {
	key_table = malloc(sizeof(*key_table));
	key_table->table = dictionary_create();
	pthread_mutex_init(&key_table->lock, NULL);
}

__attribute__((destructor)) void destroy_key_table(void) {
	dictionary_destroy_and_destroy_elements(key_table->table, free);
	pthread_mutex_destroy(&key_table->lock);
	free(key_table);
}

void key_table_create_key(char *key, struct instance_t *instance)
{
	synchronized(key_table->lock) {
		struct key_table_data_t *data = key_table_get(key);
		if (data == NULL) {
			data = key_table_data_create(instance);
			dictionary_put(key_table->table, key, data);
		} else if (instance != NULL) {
			data->instance = instance;
		}
	}
}

static struct key_table_data_t *key_table_get(char *key)
{
	return dictionary_get(key_table->table, key);
}

void key_table_remove(char *key)
{
	synchronized(key_table->lock) {
		dictionary_remove_and_destroy(key_table->table, key, free);
	}
}

struct instance_t *key_table_get_instance(char *key)
{
	struct key_table_data_t *data;
	synchronized(key_table->lock) {
		 data = key_table_get(key);
	}

	return data != NULL ? data->instance : NULL;
}

bool key_table_is_new(char *key)
{
	struct key_table_data_t *data = key_table_get(key);
	if (data != NULL) {
		return !data->initialized;
	} else {
		return false;
	}
}

bool key_table_set_initialized(char *key)
{
	struct key_table_data_t *data = key_table_get(key);

	if (data != NULL) {
		data->initialized = true;
		return true;
	} else {
		return false;
	}
}

char **key_table_get_instance_key_list(struct instance_t *instance, size_t *key_list_size)
{
	char **key_list = NULL;
	*key_list_size = 0;

	void string_array_add(char *elem) {
		*key_list_size += 1;
		key_list = realloc(key_list, sizeof(char *) * *key_list_size);
		key_list[*key_list_size - 1] = elem;
	}

	void add_instance_key(char *key, void *_data) {
		struct key_table_data_t *data = (struct key_table_data_t *)_data;
		if (data->instance == instance) {
			string_array_add(key);
		}
	}

	synchronized(key_table->lock) {
		dictionary_iterator(key_table->table, add_instance_key);
	}

	return key_list;
}

static struct key_table_data_t *key_table_data_create(struct instance_t *instance)
{
	struct key_table_data_t *data = malloc(sizeof(*data));
	data->instance = instance;
	data->initialized = false;
	return data;
}

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <commons/collections/dictionary.h>

#include "key-table.h"

struct key_table_data_t {
	char *instance_name;
};

struct key_table_t {
	t_dictionary *table;
};

static struct key_table_t *key_table;

__attribute__((constructor)) void init_key_table(void) {
	key_table->table = dictionary_create();
}

__attribute__((destructor)) void destroy_key_table(void) {
	// TODO: Destroy key table.
}

bool key_table_create_key(char *key)
{
	struct key_table_data_t *data = malloc(sizeof(*data));
	data->instance_name = strdup(key);

	if (!dictionary_has_key(key_table->table, key)) {
		dictionary_put(key_table->table, key, data);
		return true;
	} else {
		return false;
	}
}

struct key_table_data_t *key_table_get(char *key)
{
	return dictionary_get(key_table->table, key);
}

char *key_table_get_instance_name(char *key)
{
	struct key_table_data_t *data = key_table_get(key);
	return data != NULL ? data->instance_name : NULL;
}

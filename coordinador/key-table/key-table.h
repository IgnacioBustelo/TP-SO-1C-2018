#ifndef COORDINADOR_KEY_TABLE_KEY_TABLE_H_
#define COORDINADOR_KEY_TABLE_KEY_TABLE_H_

#include "../instance-list/instance-list.h"

bool key_table_create_key(char *key, struct instance_t *instance);
void key_table_remove(char *key);
struct instance_t *key_table_get_instance(char *key);
bool key_table_is_new(char *key);
bool key_table_set_initialized(char *key);
char **key_table_get_instance_key_list(struct instance_t *instance, size_t *list_size);

#endif /* COORDINADOR_KEY_TABLE_KEY_TABLE_H_ */

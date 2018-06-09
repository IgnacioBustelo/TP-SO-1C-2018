#ifndef COORDINADOR_KEY_TABLE_KEY_TABLE_H_
#define COORDINADOR_KEY_TABLE_KEY_TABLE_H_

#include "../instance-list/instance-list.h"

bool key_table_create_key(char *key, struct instance_t *instance);
struct key_table_data_t *key_table_get(char *key);
struct instance_t *key_table_get_instance(char *key);

#endif /* COORDINADOR_KEY_TABLE_KEY_TABLE_H_ */

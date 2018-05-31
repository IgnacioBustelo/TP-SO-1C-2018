#ifndef COORDINADOR_KEY_TABLE_KEY_TABLE_H_
#define COORDINADOR_KEY_TABLE_KEY_TABLE_H_

bool key_table_create_key(char *key);
struct key_table_data_t *key_table_get(char *key);
char *key_table_get_instance_name(char *key);

#endif /* COORDINADOR_KEY_TABLE_KEY_TABLE_H_ */

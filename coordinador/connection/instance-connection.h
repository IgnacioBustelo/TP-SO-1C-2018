#ifndef COORDINADOR_CONNECTION_INSTANCE_CONNECTION_H_
#define COORDINADOR_CONNECTION_INSTANCE_CONNECTION_H_

extern struct instance_list_t *instance_list;

void handle_instance_connection(int fd);
bool instance_request_value(struct instance_t *instance, char *key, char **value);

#endif /* COORDINADOR_CONNECTION_INSTANCE_CONNECTION_H_ */

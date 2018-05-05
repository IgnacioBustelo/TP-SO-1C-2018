#ifndef COORDINADOR_INSTANCE_REQUEST_LIST_H_
#define COORDINADOR_INSTANCE_REQUEST_LIST_H_

#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/queue.h>

enum request_type_t {
	INSTANCE_SET, INSTANCE_STORE
};

struct request_node_t {
	enum request_type_t type;
	union {
		struct {
			char* key;
			char* value;
		} set;
		struct {
			char* key;
		} store;
	};
};

struct request_list_t {
	t_queue *elements;
	sem_t count;
	pthread_mutex_t lock;
};

struct request_list_t *request_list_create(void);
void request_list_destroy(struct request_list_t *list);
struct request_node_t *request_list_push_set(struct request_list_t *request_list, char *key, char *value);
struct request_node_t *request_list_push_store(struct request_list_t *request_list, char *key);
struct request_node_t *request_list_pop(struct request_list_t *request_list);

#endif /* COORDINADOR_INSTANCE_REQUEST_LIST_H_ */

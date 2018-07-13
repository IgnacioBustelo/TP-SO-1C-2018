#ifndef COORDINADOR_INSTANCE_REQUEST_LIST_H_
#define COORDINADOR_INSTANCE_REQUEST_LIST_H_

#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/queue.h>

enum request_type_t {
	INSTANCE_SET, INSTANCE_STORE
};

struct request_node_t {
	int requesting_esi_fd;
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
	t_list *excluded_instances;
};

struct request_list_t {
	t_queue *elements;
	sem_t count;
	pthread_mutex_t lock;
};

struct request_list_t *request_list_create(void);
void request_list_destroy(struct request_list_t *list);
struct request_node_t *request_list_push_set(struct request_list_t *request_list, int esi_fd, char *key, char *value);
struct request_node_t *request_list_push_store(struct request_list_t *request_list, int esi_fd, char *key);
void request_list_push(struct request_list_t *request_list, struct request_node_t *elem);
struct request_node_t *request_list_pop(struct request_list_t *request_list);
void request_list_iterate(struct request_list_t *request_list, void (*closure)(void *));
void request_node_destroy(struct request_node_t *node);

#endif /* COORDINADOR_INSTANCE_REQUEST_LIST_H_ */

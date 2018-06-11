#ifndef COORDINADOR_INSTANCE_LIST_H_
#define COORDINADOR_INSTANCE_LIST_H_

#include <pthread.h>
#include <semaphore.h>

#include <commons/collections/list.h>

#define DISCONNECTED -1

struct instance_list_t {
	pthread_mutex_t lock;
	t_list *elements;
};

struct instance_t {
	char *name;
	int fd;
	size_t used_entries;
	struct request_list_t *requests;
};

struct instance_list_t *instance_list_create(void);
void instance_list_destroy(struct instance_list_t *victim);
struct instance_t *instance_list_get(struct instance_list_t *instance_list, char *name);
struct instance_t *instance_list_add(struct instance_list_t *instance_list, char *name, int fd);
struct instance_t* instance_list_remove(struct instance_list_t *instance_list, char *name);
struct instance_t *instance_list_first(struct instance_list_t *instance_list);
struct instance_t *instance_list_push(struct instance_list_t *instance_list, struct instance_t *elem);
struct instance_t *instance_list_pop(struct instance_list_t *instance_list);
void instance_list_sort(struct instance_list_t *instance_list, bool (*comparator)(void *, void *));
bool instance_list_delete(struct instance_list_t *instance_list, char *name);

#endif /* COORDINADOR_INSTANCE_LIST_H_ */

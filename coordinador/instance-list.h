#ifndef COORDINADOR_INSTANCE_LIST_H_
#define COORDINADOR_INSTANCE_LIST_H_

#include <semaphore.h>

struct instance_t {
	int fd;
	t_list *requests;
	sem_t requests_count;
};

t_list *instance_list_create(void);
struct instance_t *instance_list_add(t_list *instance_list, int fd);
struct instance_t* instance_list_remove(t_list* instance_list, int fd);
bool instance_list_delete(t_list *instance_list, int fd);

#endif /* COORDINADOR_INSTANCE_LIST_H_ */

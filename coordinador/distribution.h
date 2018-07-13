#ifndef COORDINADOR_DISTRIBUTION_H_
#define COORDINADOR_DISTRIBUTION_H_

#include <commons/collections/list.h>

#include "instance-list/instance-list.h"

struct instance_t *dispatch(struct instance_list_t *instance_list, char *key, t_list *excluded_instances);

#endif /* COORDINADOR_DISTRIBUTION_H_ */

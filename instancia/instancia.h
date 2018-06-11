#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

#include <commons/collections/list.h>

#include "globals.h"

typedef enum {
	INSTANCE_SUCCESS = 1,
	INSTANCE_SET_ERROR = 0
} instancia_status;

void	instance_init(char* process_name, char* logger_route, char* log_level, char* cfg_route);

int		instance_set(key_value_t* key_value, t_list* replaced_keys);

int		instance_store(char* key);

int		instance_dump(t_list* stored_keys);

int		instance_compact();

void	instance_main();

void	instance_show();

void	instance_die();

#endif

#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

#include <commons/collections/list.h>

#include "globals.h"

typedef enum {
	INSTANCE_INIT_SUCCESS,
	INSTANCE_INIT_ERROR,
	INSTANCE_HANDSHAKE_SUCCESS,
	INSTANCE_HANDSHAKE_REJECTED,
	INSTANCE_REQUEST_SUCCESS,
	INSTANCE_REQUEST_FAILURE,
	INSTANCE_COMPACT,
	INSTANCE_API_ERROR,
	INSTANCE_DIE
} request_result;

int		instance_init(char* process_name, char* logger_route, char* log_level, char* cfg_route);

int		instance_handshake(storage_setup_t* setup, t_list** recoverable_keys);

int		instance_set(key_value_t* key_value, t_list* replaced_keys);

int		instance_store(char* key);

int		instance_status(char* key, key_value_t** key_value);

int		instance_recover(t_list* recoverable_keys);

void	instance_thread_api(void* args);

void	instance_thread_dump(void* args);

void	instance_main();

void	instance_show();

void	instance_die();

#endif

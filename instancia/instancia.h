#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

#include "globals.h"

void	instance_init(char* process_name, char* logger_route, char* log_level, char* cfg_route);

void	instance_set(key_value_t* key_value);

int		instance_store(char* key);

void	instance_main();

void	instance_show();

void	instance_die();

#endif

#ifndef INSTANCIA_CFG_INSTANCIA_H_
#define INSTANCIA_CFG_INSTANCIA_H_

#include "../libs/messenger.h"

#define FIELDS 6

void	cfg_instancia_init(char* config_path);

char*	cfg_instancia_get_coordinador_ip();

int		cfg_instancia_get_coordinador_port();

char*	cfg_instancia_get_replacement_algorithm_name();

void	cfg_instancia_invoke_replacement_algorithm();

char*	cfg_instancia_get_mount_point();

char*	cfg_instancia_get_instance_name();

int		cfg_instancia_get_dump_time();

void	cfg_instancia_destroy();

#endif

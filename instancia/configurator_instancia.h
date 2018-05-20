#ifndef INSTANCIA_CONFIGURATOR_INSTANCIA_H_
#define INSTANCIA_CONFIGURATOR_INSTANCIA_H_

#include "../libs/configurator.h"

char*	configurator_get_coordinador_ip();

int		configurator_get_coordinador_port();

char*	configurator_get_replacement_algorithm_name();

void	configurator_invoke_replacement_algorithm();

char*	configurator_get_mount_point();

char*	configurator_get_instance_name();

int		configurator_get_dump_time();

#endif

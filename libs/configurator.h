#ifndef INSTANCIA_CONFIG_H_
#define INSTANCIA_CONFIG_H_

#include <commons/config.h>

#include "messenger.h"

t_config* config;

void configurator_init(char* config_path, char** fields, size_t fields_size);

void configurator_destroy();

#endif

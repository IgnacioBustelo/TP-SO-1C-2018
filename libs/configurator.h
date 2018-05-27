#ifndef INSTANCIA_CONFIG_H_
#define INSTANCIA_CONFIG_H_

#include <commons/collections/list.h>
#include <commons/config.h>
#include <stdlib.h>

t_list* config_field_list;

t_config* config;

void configurator_init(char* config_path, char** fields, size_t fields_size);

void configurator_read();

void configurator_destroy();

#endif

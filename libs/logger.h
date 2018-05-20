#ifndef INSTANCIA_LOGGER_H_
#define INSTANCIA_LOGGER_H_

#include <commons/log.h>
#include "messenger.h"

t_log* logger;

void logger_init(char* logger_route, char* level);

void logger_destroy();

#endif

#ifndef INSTANCIA_EVENT_HANDLER_H_
#define INSTANCIA_EVENT_HANDLER_H_

#include <stdbool.h>

bool instance_is_alive, instance_requires_dump;

void event_handler_init();

void event_handler_action(int signal);

void event_handler_alarm(int time);

#endif

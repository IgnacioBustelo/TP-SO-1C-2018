#ifndef INSTANCIA_EVENT_HANDLER_H_
#define INSTANCIA_EVENT_HANDLER_H_

#include <stdbool.h>

bool instance_is_alive;

void event_handler_init();

void event_handler_init_with_finisher(void(*finisher)());

void event_handler_action(int signal);

#endif

#ifndef LIBS_MESSENGERS_MESSENGER_H_
#define LIBS_MESSENGERS_MESSENGER_H_

#include <stdarg.h>
#include <stdlib.h>

void	messenger_init(char* route, char* process_name, char* log_level);

void	messenger_show(char* log_level, const char* format,...);

void	messenger_show_method(char* log_level, char* message);

char*	messenger_bytes_to_string(void* data, size_t size);

void	messenger_destroy();

#endif

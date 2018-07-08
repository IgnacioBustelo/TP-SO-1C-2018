#ifndef LIBS_MESSENGERS_MESSENGER_H_
#define LIBS_MESSENGERS_MESSENGER_H_

#include <commons/collections/list.h>
#include <stdarg.h>
#include <stdlib.h>

void	messenger_init(char* route, char* process_name, char* log_level);

void	messenger_show(char* log_level, const char* format,...);

void	messenger_show_method(char* log_level, char* message);

char*	messenger_bytes_to_string(void* data, size_t size);

char*	messenger_list_to_string(t_list* string_list);

int		messenger_longest_string_length(int max_number);

void	messenger_destroy();

#endif

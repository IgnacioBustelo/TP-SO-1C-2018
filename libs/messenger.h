#ifndef LIBS_MESSENGERS_MESSENGER_H_
#define LIBS_MESSENGERS_MESSENGER_H_

#include <stdlib.h>

void messenger_show(char* log_level, const char* format,...);

void messenger_show_method(char* log_level, char* message);

char* messenger_bytes_to_string(void* data, size_t size);

void messenger_log(char* message, char* level); // TODO: Cambiar los messenger_log por messenger_show + template de mensaje

#endif

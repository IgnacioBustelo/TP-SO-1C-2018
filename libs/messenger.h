#ifndef LIBS_MESSENGERS_MESSENGER_H_
#define LIBS_MESSENGERS_MESSENGER_H_

void messenger_show(char* log_level, const char* format,...);

void messenger_show_method(char* log_level, char* message);

void messenger_log(char* message, char* level); // TODO: Cambiar los messenger_log por messenger_show + template de mensaje

#endif

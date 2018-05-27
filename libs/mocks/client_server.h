#ifndef LIBS_MOCKS_CLIENT_SERVER_H_
#define LIBS_MOCKS_CLIENT_SERVER_H_

#define HOST "127.0.0.1"
#define PORT 8080

#include "../messenger.h"

char *server_name, *client_name;

void client_server_run();

void client_server_execute_server(int fd_client);

void client_server_execute_client(int fd_server);

void client_server_messenger_show_color(char* color);

#endif

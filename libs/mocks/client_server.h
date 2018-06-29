#ifndef LIBS_MOCKS_CLIENT_SERVER_H_
#define LIBS_MOCKS_CLIENT_SERVER_H_

#include "../messenger.h"
#include "color.h"

#define HOST			"127.0.0.1"
#define PORT 			8080
#define COLOR_SERVER	COLOR_GREEN
#define COLOR_CLIENT	COLOR_CYAN

char *server_name, *client_name;

void client_server_run();

void client_server_execute_server(int fd_client);

void client_server_execute_client(int fd_server);

#endif

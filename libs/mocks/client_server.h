#ifndef LIBS_MOCKS_CLIENT_SERVER_H_
#define LIBS_MOCKS_CLIENT_SERVER_H_

#define HOST "127.0.0.1"
#define PORT 8080

char* server_name;
char* client_name;

void client_server_run();

void client_server_execute_server(int fd_client);

void client_server_execute_client(int fd_server);

#endif

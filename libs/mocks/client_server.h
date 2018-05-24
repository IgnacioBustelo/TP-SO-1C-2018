#ifndef LIBS_MOCKS_CLIENT_SERVER_H_
#define LIBS_MOCKS_CLIENT_SERVER_H_

void client_server_run(char* ip, int port);

void client_server_execute_client(int fd_server, int fd_client);

void client_server_execute_server(int fd_server);

#endif

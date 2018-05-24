#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#include "../messenger.h"
#include "../conector.h"
#include "client_server.h"

#define HOST "127.0.0.1"
#define PORT 8080

sem_t server_sem;
sem_t client_sem;

char* server_name;
char* client_name;

static void set_server(void* args) {
	sem_wait(&server_sem);

	int port = (int) args;

	int fd_server = init_listener(PORT, 1);

	messenger_show("INFO", "%s: Se levanto el servidor", server_name);

	sem_post(&client_sem);

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;

	int fd_client = accept(fd_server, (struct sockaddr *) &client_info, &addrlen);

	messenger_show("INFO", "%s: Se acepto a %s", server_name, client_name);

	client_server_execute_server(fd_server, fd_client);

	messenger_show("INFO", "%s: Se termino la conexion con %s", server_name, client_name);

	close(fd_server);
	close(fd_client);

	pthread_exit(NULL);
}

static void set_client() {
	sem_wait(&server_sem);

	int fd_server = connect_to_server(HOST, PORT);

	messenger_show("INFO", "%s: Se conecto a %s", client_name, server_name);

	client_server_execute_client(fd_server);

	messenger_show("INFO", "%s: Se termino la conexion con %s", client_name, server_name);

	close(fd_server);

	pthread_exit(NULL);
}

void client_server_run(char* ip, int port) {
	sem_init(&server_sem, 1);
	sem_init(&client_sem, 0);

	pthread_t server_thread, client_thread;

	pthread_create(&server_thread, NULL, (void*) set_server, NULL);
	pthread_create(&client_thread, NULL, (void*) set_client, NULL);

	pthread_join(server_thread, NULL);
	pthread_join(server_thread, NULL);

	sem_destroy(&server_sem);
	sem_destroy(&client_sem);
}

#include <pthread.h>
#include <stdio.h>

#include "../messenger.h"
#include "../conector.h"
#include "client_server.h"

static void set_server(void* args) {
	sem_wait(&server_sem);

	int fd_server = init_listener(PORT, 1);

	messenger_show("INFO", "%s: Se levanto el servidor", server_name);

	sem_post(&client_sem);

	sem_wait(&server_sem);

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;

	int fd_client = accept(fd_server, (struct sockaddr *) &client_info, &addrlen);

	messenger_show("INFO", "%s: Se acepto a %s", server_name, client_name);

	sem_post(&server_sem);

	client_server_show_semaphores();

	client_server_execute_server(fd_client);

	sem_wait(&server_sem);

	messenger_show("INFO", "%s: Se termino la conexion con %s", server_name, client_name);

	sem_post(&client_sem);

	close(fd_server);
	close(fd_client);

	pthread_exit(NULL);
}

static void set_client() {
	sem_wait(&client_sem);

	int fd_server = connect_to_server(HOST, PORT);

	messenger_show("INFO", "%s: Se conecto a %s", client_name, server_name);

	sem_post(&server_sem);

	client_server_execute_client(fd_server);

	sem_wait(&client_sem);

	messenger_show("INFO", "%s: Se termino la conexion con %s", client_name, server_name);

	sem_post(&server_sem);

	close(fd_server);

	pthread_exit(NULL);
}

void client_server_run() {
	sem_init(&server_sem, 0, 1);
	sem_init(&client_sem, 0, 0);

	pthread_t server_thread, client_thread;

	client_server_show_semaphores();

	pthread_create(&server_thread, NULL, (void*) set_server, NULL);
	pthread_create(&client_thread, NULL, (void*) set_client, NULL);

	pthread_join(server_thread, NULL);
	pthread_join(server_thread, NULL);

	client_server_show_semaphores();

	sem_destroy(&server_sem);
	sem_destroy(&client_sem);
}

void client_server_show_semaphores() {
	int server_sem_value, client_sem_value;

	sem_getvalue(&server_sem, &server_sem_value);
	sem_getvalue(&client_sem, &client_sem_value);

	messenger_show("DEBUG", "General: Estado de semaforos: Servidor = %d ; Cliente = %d", server_sem_value, client_sem_value);
}

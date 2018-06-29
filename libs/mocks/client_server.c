#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

#include "../mocks/printfer.h"
#include "../conector.h"
#include "client_server.h"

#define COLOR_SERVER COLOR_GREEN
#define COLOR_CLIENT COLOR_CYAN

pthread_t server_thread, client_thread;
sem_t server_sem, client_sem;

char* color_get() {
	pthread_t current_thread_id = pthread_self();

	bool is_server = pthread_equal(current_thread_id, server_thread);
	bool is_client = pthread_equal(current_thread_id, client_thread);

	return (is_server) ? COLOR_SERVER : (is_client) ? COLOR_CLIENT : COLOR_RESET;
}

static void set_server(void* args) {
	is_closable_client = true;

	sem_wait(&server_sem);

	int fd_server = init_listener(PORT, 1);

	messenger_show("INFO", "Se inicio el servidor %s", server_name);

	sem_post(&client_sem);

	sem_wait(&server_sem);

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;

	int fd_client = accept(fd_server, (struct sockaddr *) &client_info, &addrlen);

	messenger_show("INFO", "Se acepto al cliente %s", client_name);

	sem_post(&client_sem);

	client_server_execute_server(fd_client);

	if(is_closable_server) {
		close(fd_server);
		close(fd_client);
	}
}

static void set_client(void* args) {
	is_closable_client = true;

	sem_wait(&client_sem);

	messenger_show("INFO", "Se inicio el cliente %s", client_name);

	int fd_server = connect_to_server(HOST, PORT);

	messenger_show("INFO", "Se conecto al servidor %s", server_name);

	sem_post(&server_sem);

	sem_wait(&client_sem);

	client_server_execute_client(fd_server);

	if(is_closable_client)  {
		close(fd_server);
	}
}

void client_server_run() {
	sem_init(&server_sem, 0, 1);
	sem_init(&client_sem, 0, 0);

	pthread_create(&server_thread, NULL, (void*) set_server, NULL);
	pthread_create(&client_thread, NULL, (void*) set_client, NULL);

	pthread_join(server_thread, NULL);
	pthread_join(client_thread, NULL);

	messenger_show("INFO", "Se cerraron las conexiones");

	sem_destroy(&server_sem);
	sem_destroy(&client_sem);
}

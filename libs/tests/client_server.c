#include <commons/string.h>
#include <semaphore.h>
#include <stdio.h>

#include "../messenger.h"
#include "../mocks/client_server.h"
#include "../mocks/color.h"

sem_t execute_server, execute_client;

void client_server_execute_server(int fd_client) {
	sem_wait(&execute_server);

	messenger_show("INFO", "Soy el servidor %s", server_name);

	sem_post(&execute_client);
}

void client_server_execute_client(int fd_server) {
	sem_wait(&execute_client);

	messenger_show("INFO", "Soy el cliente %s", client_name);

	sem_post(&execute_server);
}

int main(int argc, char* argv[]) {
	sem_init(&execute_server, 0, 1);
	sem_init(&execute_client, 0, 0);

	server_name = (argc < 2) ? "S" : argv[1];

	client_name = (argc < 3) ? "C" : argv[2];

	client_server_run();

	sem_destroy(&execute_server);
	sem_destroy(&execute_client);
}

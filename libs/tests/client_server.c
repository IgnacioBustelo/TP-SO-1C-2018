#include "../messenger.h"
#include "../mocks/client_server.h"

void client_server_execute_client(int fd_server) {
	sem_wait(&client_sem);

	messenger_show("INFO", "Cliente: Soy el %s", client_name);

	sem_post(&server_sem);
}

void client_server_execute_server(int fd_server) {
	sem_wait(&server_sem);

	messenger_show("INFO", "Servidor: Soy el %s", server_name);

	sem_post(&client_sem);
}

int main(int argc, char* argv[]) {
	server_name = (argc < 2) ? "Servidor" : argv[1];

	client_name = (argc < 3) ? "Cliente" : argv[2];

	client_server_run();
}

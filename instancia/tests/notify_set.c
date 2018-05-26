#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"

int status, entries_used;

void client_server_execute_server(int fd_client) {
	int header, status_received, entries_used_received;

	chunk_recv(fd_client, &header, sizeof(status_received));
	chunk_recv(fd_client, &status_received, sizeof(status_received));
	chunk_recv(fd_client, &entries_used_received, sizeof(entries_used_received));

	messenger_show("INFO", "Se recibio la operacion %d con un status %d y en la instancia se usan %d entradas", header, status_received, entries_used_received);
}

void client_server_execute_client(int fd_server) {
	fd_coordinador = fd_server;

	coordinator_api_notify_set(status, entries_used);
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";
	client_name = "Instancia 1";

	status = (argc < 2) ? 1 : atoi(argv[1]);
	entries_used = (argc < 3) ? 16 : atoi(argv[2]);

	client_server_run();
}

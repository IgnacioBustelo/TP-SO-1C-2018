#include "../../libs/mocks/client_server.h"
#include "../coordinator_api.h"
#include "coordinador_mock.h"

int instance_is_alive, entries_used;

void client_server_execute_server(int fd_client) {
	coordinador_mock_set_response(fd_client);
}

void client_server_execute_client(int fd_server) {
	fd_coordinador = fd_server;

	coordinator_api_notify_set(instance_is_alive, entries_used);
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";
	client_name = "Instancia 1";

	instance_is_alive = (argc < 2) ? 1 : atoi(argv[1]);
	entries_used = (argc < 3) ? 16 : atoi(argv[2]);

	client_server_run();
}

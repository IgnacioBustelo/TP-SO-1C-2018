#include "../../libs/mocks/client_server.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "coordinador_mock.h"

char *key, *value;

void client_server_execute_server(int fd_client) {
	coordinador_mock_set_request(fd_client, key, value);
}

void client_server_execute_client(int fd_server) {
	fd_coordinador = fd_server;

	coordinator_api_receive_header();

	key_value_destroy(coordinator_api_receive_set());
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";
	client_name = "Instancia 1";

	key = "A";
	value = "Test";

	client_server_run();
}

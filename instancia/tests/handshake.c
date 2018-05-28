#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "coordinador_mock.h"

int total_entries, entry_size;

void client_server_execute_server(int fd_client) {
	coordinador_mock_handshake(fd_client, total_entries, entry_size);
}

void client_server_execute_client(int fd_server) {
	storage_setup_t dimensions;

	fd_coordinador = fd_server;

	coordinator_api_handshake(client_name, &dimensions);
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";

	total_entries = (argc < 2) ? 16 : atoi(argv[1]);
	entry_size = (argc < 3) ? 4 : atoi(argv[2]);
	client_name = (argc < 4) ? "Instancia 1" : argv[3];

	client_server_run();
}

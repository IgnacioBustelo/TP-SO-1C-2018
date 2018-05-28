#include "../../libs/mocks/client_server.h"
#include "../bootstrap.h"
#include "../coordinator_api.h"
#include "../entry_table.h"
#include "../instancia.h"
#include "../storage.h"
#include "coordinador_mock.h"

int total_entries, entry_size;
char *key, *value;

void client_server_execute_server(int fd_client) {
	coordinador_mock_handshake(fd_client, total_entries, entry_size);

	coordinador_mock_set_request(fd_client, key, value);

	coordinador_mock_set_response(fd_client);

	coordinador_mock_kill(fd_client);
}

void client_server_execute_client(int fd_server) {
	fd_coordinador = fd_server;

	bootstrap_start("Instancia", "../instancia.log", "INFO", "../instancia.cfg");

	instance_main();

	entry_table_print_table();

	storage_show();

	instance_die();
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";
	client_name = "Instancia 1";

	total_entries = 16;
	entry_size = 4;

	key = "A";
	value = (argc == 1) ? "Test" : argv[1];

	client_server_run();
}

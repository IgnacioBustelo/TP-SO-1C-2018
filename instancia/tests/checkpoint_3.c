#include <commons/string.h>

#include "../../libs/mocks/client_server.h"
#include "../coordinator_api.h"
#include "../entry_table.h"
#include "../instancia.h"
#include "../storage.h"
#include "coordinador_mock.h"

int		total_entries, entry_size, key_amount;
char**	values;

void client_server_execute_server(int fd_client) {
	coordinador_mock_handshake(fd_client, total_entries, entry_size);

	int i;

	for(i = 1; i < key_amount; i++) {
		char* key = string_from_format("clave_%d", i);

		coordinador_mock_set_request(fd_client, key, values[i]);

		coordinador_mock_set_response(fd_client);

		coordinador_mock_store_request(fd_client, key);

		coordinador_mock_store_response(fd_client);

		free(key);
	}

	coordinador_mock_kill(fd_client);
}

void client_server_execute_client(int fd_server) {
	fd_coordinador = fd_server;

	instance_init("Instancia", "../instancia.log", "INFO", "../instancia.cfg");

	instance_main();

	instance_show();

	instance_die();
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";
	client_name = "Instancia 1";

	total_entries = 8;
	entry_size = 4;
	key_amount = argc;

	values = argv;

	client_server_run();
}

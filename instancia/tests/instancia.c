#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>

#include "../../libs/mocks/client_server.h"
#include "../../libs/mocks/printfer.h"

#include "../coordinator_api.h"
#include "../instancia.h"
#include "coordinador_mock.h"

int		total_entries, entry_size, key_amount;
char	**keys, **values;
t_list	*recoverable_keys;

void client_server_execute_server(int fd_client) {
	char* received_name;

	bool is_accepted = key_amount > 1;

	coordinador_mock_handshake_base(fd_client, &is_accepted);

	if(!is_accepted) {
		pthread_exit(NULL);
	}

	coordinador_mock_handshake_receive_name(fd_client, &received_name);

	coordinador_mock_handshake_send_config(fd_client, received_name, 16, 4, recoverable_keys);

	free(received_name);

	int i, et_initial_size = list_size(entry_table);

	for(i = 1; i < key_amount; i++) {
		coordinador_mock_check_request(fd_client);

		coordinador_mock_check_response(fd_client);

		coordinador_mock_set_request(fd_client, false, keys[i - 1], values[i]);

		coordinador_mock_set_response(fd_client);

		coordinador_mock_store_request(fd_client, keys[i - 1]);

		coordinador_mock_store_response(fd_client);

		coordinador_mock_status_request(fd_client, keys[i - 1]);

		free(coordinador_mock_status_response(fd_client, keys[i - 1]));
	}

	coordinador_mock_kill(fd_client);
}

void client_server_execute_client(int fd_server) {
	is_closable_client = false;

	fd_coordinador = fd_server;

	instance_init("Instancia", "../instancia.log", "INFO", "../instancia.cfg");

	instance_main();

	instance_die();
}

int main(int argc, char* argv[]) {
	printfer_set_levels(false, true);

	server_name = "Coordinador";
	client_name = "Instancia 1";

	total_entries = 8;
	entry_size = 4;
	key_amount = argc;

	keys = calloc(key_amount, sizeof(char*));
	values = argv;

	recoverable_keys = list_create();

	int i;
	for(i = 1; i < key_amount; i++) {
		keys[i - 1] = string_from_format("clave_%d", i);

		list_add(recoverable_keys, keys[i - 1]);
	}

	client_server_run();

	list_destroy_and_destroy_elements(recoverable_keys, free);

	free(keys);
}

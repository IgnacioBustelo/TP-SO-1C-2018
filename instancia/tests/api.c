#include <pthread.h>

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/color.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "coordinador_mock.h"

bool	is_accepted;
t_list* recoverable_keys;

void client_server_execute_server(int fd_client) {
	messenger_show("INFO", "%sTest Handshake%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_handshake(fd_client, is_accepted, 4, 16, recoverable_keys);

	if(!is_accepted) {
		pthread_exit(NULL);
	}

	messenger_show("INFO", "%sTest SET%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_set_request(fd_client, "A", "Test");

	coordinador_mock_set_response(fd_client);

	messenger_show("INFO", "%sTest STORE%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_store_request(fd_client, "A");

	coordinador_mock_store_response(fd_client);
}

void client_server_execute_client(int fd_server) {
	storage_setup_t dimensions;
	t_list* recoverable_keys_received;

	fd_coordinador = fd_server;

	int status = coordinator_api_handshake(client_name, &dimensions, &recoverable_keys_received);

	if(status == API_HANDSHAKE_ERROR) {
		pthread_exit(NULL);
	}

	coordinator_api_receive_header();

	key_value_destroy(coordinator_api_receive_set());

	coordinator_api_notify_set(STATUS_COMPACT, 0);

	coordinator_api_receive_header();

	free(coordinator_api_receive_store());

	coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, STATUS_OK);

	list_destroy_and_destroy_elements(recoverable_keys_received, free);
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";
	client_name = "Instancia 1";

	is_accepted = argc == 1;

	char *key_1 = "Test_1", *key_2 = "Test_2", *key_3 = "Test 3";

	recoverable_keys = list_create();

	list_add(recoverable_keys, key_1);
	list_add(recoverable_keys, key_2);
	list_add(recoverable_keys, key_3);

	client_server_run();

	list_destroy(recoverable_keys);
}

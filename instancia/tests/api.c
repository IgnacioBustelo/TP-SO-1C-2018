#include <pthread.h>

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/color.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "coordinador_mock.h"

bool is_accepted;

void client_server_execute_server(int fd_client) {
	messenger_show("INFO", "%sTest Handshake%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_handshake(fd_client, is_accepted, 4, 16);

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

	fd_coordinador = fd_server;

	int status = coordinator_api_handshake(client_name, &dimensions);

	if(status == HANDSHAKE_ERROR) {
		pthread_exit(NULL);
	}

	coordinator_api_receive_header();

	key_value_destroy(coordinator_api_receive_set());

	coordinator_api_notify_set(STATUS_COMPACT, 0);

	coordinator_api_receive_header();

	free(coordinator_api_receive_store());

	coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, STATUS_OK);
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";
	client_name = "Instancia 1";

	is_accepted = argc == 1;

	client_server_run();
}

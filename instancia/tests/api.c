#include <pthread.h>
#include <semaphore.h>

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/color.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "coordinador_mock.h"

sem_t handshake_sem;

bool	is_accepted;
t_list* recoverable_keys;

void client_server_execute_server(int fd_client) {
	messenger_show("INFO", "%sTest Handshake%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_handshake(fd_client, is_accepted, 4, 16, recoverable_keys);

	if(!is_accepted) {
		pthread_exit(NULL);
	}

	sem_wait(&handshake_sem);

	messenger_show("INFO", "%sTest SET%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_set_request(fd_client, true, "A", "Test");

	coordinador_mock_set_response(fd_client);

	coordinador_mock_set_request(fd_client, false, "B", "Test");

	coordinador_mock_set_response(fd_client);

	messenger_show("INFO", "%sTest STORE%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_store_request(fd_client, "A");

	coordinador_mock_store_response(fd_client);

	messenger_show("INFO", "%sTest STATUS%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_status_request(fd_client, "A");

	free(coordinador_mock_status_response(fd_client, "A"));
}

void client_server_execute_client(int fd_server) {
	storage_setup_t dimensions;
	t_list* recoverable_keys_received;
	bool is_new_key;

	fd_coordinador = fd_server;

	int status = coordinator_api_handshake(client_name, &dimensions, &recoverable_keys_received);

	if(status == API_HANDSHAKE_ERROR) {
		pthread_exit(NULL);
	}

	sem_post(&handshake_sem);

	coordinator_api_receive_header();

	key_value_destroy(coordinator_api_receive_set(&is_new_key));

	coordinator_api_notify_set(STATUS_COMPACT, 0);

	coordinator_api_receive_header();

	key_value_destroy(coordinator_api_receive_set(&is_new_key));

	coordinator_api_notify_set(STATUS_NO_SPACE, 5);

	coordinator_api_receive_header();

	free(coordinator_api_receive_key());

	coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, STATUS_REPLACED);

	coordinator_api_receive_header();

	char* requested_key = coordinator_api_receive_key();

	key_value_t* key_value = key_value_create(requested_key, "Status_Test");

	coordinator_api_notify_key_value(STATUS_OK, key_value);

	key_value_destroy(key_value);

	free(requested_key);

	list_destroy_and_destroy_elements(recoverable_keys_received, free);
}

int main(int argc, char* argv[]) {
	sem_init(&handshake_sem, 0, 0);

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

	sem_destroy(&handshake_sem);
}

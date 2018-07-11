#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>

#include "../../libs/mocks/color.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/mocks/printfer.h"

#include "../../libs/chunker.h"
#include "../../libs/conector.h"
#include "../../libs/messenger.h"

#include "../coordinator_api.h"
#include "../globals.h"

#include "coordinador_mock.h"

#define	CHECK if(status == API_ERROR) pthread_exit(NULL);

bool	is_accepted;
t_list* recoverable_keys;

void client_server_execute_server(int fd_client) {
	messenger_show("INFO", "%sTest Handshake%s", COLOR_MAGENTA, COLOR_SERVER);

	char* received_name;

	coordinador_mock_handshake_base(fd_client, &is_accepted);

	if(!is_accepted) {
		pthread_exit(NULL);
	}

	coordinador_mock_handshake_receive_name(fd_client, &received_name);

	coordinador_mock_handshake_send_config(fd_client, received_name, 16, 4, recoverable_keys);

	free(received_name);

	messenger_show("INFO", "%sTest Chequeo de conexion%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_check_request(fd_client);

	coordinador_mock_check_response(fd_client);

	messenger_show("INFO", "%sTest SET clave nueva%s", COLOR_MAGENTA, COLOR_SERVER);

	coordinador_mock_set_request(fd_client, true, "A", "Test");

	coordinador_mock_set_response(fd_client);

	messenger_show("INFO", "%sTest SET%s", COLOR_MAGENTA, COLOR_SERVER);

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
	int status;

	request_coordinador header;
	storage_setup_t setup_received;
	t_list* recoverable_keys_received;
	bool is_confirmed, is_new_key;
	key_value_t *key_value;
	char *key;

	fd_coordinador = fd_server;

	status = coordinator_api_handshake_base(&is_confirmed); CHECK

	if(!is_confirmed) {
		pthread_exit(NULL);
	}

	status = coordinator_api_handshake_send_name(client_name); CHECK

	status = coordinator_api_handshake_receive_config(&setup_received, &recoverable_keys_received); CHECK

	status = coordinator_api_receive_header(&header); CHECK

	status = coordinator_api_notify_header(PROTOCOL_IC_CONFIRM_CONNECTION); CHECK

	status = coordinator_api_receive_header(&header); CHECK

	status = coordinator_api_receive_set(&is_new_key, &key_value); CHECK

	key_value_destroy(key_value);

	status = coordinator_api_notify_set(0, STATUS_COMPACT); CHECK

	status = coordinator_api_receive_header(&header); CHECK

	status = coordinator_api_receive_set(&is_new_key, &key_value); CHECK

	key_value_destroy(key_value);

	status = coordinator_api_notify_set(5, STATUS_NO_SPACE); CHECK

	status = coordinator_api_receive_header(&header); CHECK

	status = coordinator_api_receive_key(&key); CHECK

	free(key);

	status = coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, STATUS_REPLACED); CHECK

	status = coordinator_api_receive_header(&header); CHECK

	status = coordinator_api_receive_key(&key); CHECK

	key_value = key_value_create(key, "Status_Test");

	status = coordinator_api_notify_key_value(key_value, STATUS_OK); CHECK

	key_value_destroy(key_value);

	free(key);

	list_destroy_and_destroy_elements(recoverable_keys_received, free);
}

int main(int argc, char* argv[]) {
	if(argc > 1) {
		printfer_set_levels(string_equals_ignore_case(argv[1], "TRACE"), string_equals_ignore_case(argv[1], "DEBUG"));
	}

	server_name = "Coordinador";
	client_name = "Instancia_1";

	is_accepted = argc < 3;

	char *key_1 = "Test_1", *key_2 = "Test_2", *key_3 = "Test_3";

	recoverable_keys = list_create();

	list_add(recoverable_keys, key_1);
	list_add(recoverable_keys, key_2);
	list_add(recoverable_keys, key_3);

	client_server_run();

	list_destroy(recoverable_keys);
}

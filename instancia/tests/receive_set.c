#include <commons/string.h>

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"

char *key, *value;

void client_server_execute_server(int fd_client) {
	messenger_show("INFO", "Preparando clave y valor a enviar", key, value);

	chunk_t* key_value = chunk_create();

	chunk_add_variable(key_value, key, string_length(key) + 1);
	chunk_add_variable(key_value, value, string_length(value) + 1);

	messenger_show("INFO", "Enviando SET %s, %s", key, value);

	chunk_send_and_destroy(fd_client, key_value);}

void client_server_execute_client(int fd_server) {
	fd_coordinador = fd_server;

	key_value_destroy(coordinator_api_receive_set());
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";
	client_name = "Instancia 1";

	key = "A";
	value = "Test";

	client_server_run();
}

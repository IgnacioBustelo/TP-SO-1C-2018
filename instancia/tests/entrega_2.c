#include <commons/string.h>

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "../main_mock.h"

int total_entries, entry_size;

void client_server_execute_server(int fd_client) {
	char *key = "Clave", *value = "Valor";

	receive_handshake(fd_client);

	messenger_show("INFO", "%sSe recibio una solicitud de handshake de una instancia", COLOR_SERVER);

	messenger_show("INFO", "Se envio confirmacion de handshake");

	messenger_show("INFO", "Esperando recibir el nombre de la instancia%s", COLOR_CLIENT);

	send_confirmation(fd_client, true);

	char* received_name;

	chunk_recv_variable(fd_client, (void**) &received_name);

	messenger_show("INFO", "%sSe recibio el nombre de la instancia, que se llama %s", COLOR_SERVER, received_name);

	storage_setup_t setup = {.total_entries = total_entries, .entry_size = entry_size};

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &setup.entry_size, sizeof(size_t));

	chunk_add(chunk, &setup.total_entries, sizeof(size_t));

	messenger_show("INFO", "Se va a enviar el tamano del storage de %d entradas de tamano %d a %s%s", setup.total_entries, setup.entry_size, received_name, COLOR_CLIENT);

	chunk_send_and_destroy(fd_client, chunk);

	free(received_name);

	sleep(1);

	messenger_show("INFO", "%sPreparando clave y valor a enviar", COLOR_SERVER, key, value);

	chunk_t* key_value = chunk_create();

	request_coordinador header = PROTOCOL_CI_SET;

	chunk_add(key_value, &header, sizeof(header));
	chunk_add_variable(key_value, key, string_length(key) + 1);
	chunk_add_variable(key_value, value, string_length(value) + 1);

	messenger_show("INFO", "SET %s, %s%s", key, value, COLOR_CLIENT);

	chunk_send_and_destroy(fd_client, key_value);

	request_instancia response;
	int status, entries_used;

	chunk_recv(fd_client, &response, sizeof(response));
	chunk_recv(fd_client, &status, sizeof(status));
	chunk_recv(fd_client, &entries_used, sizeof(entries_used));

	messenger_show("INFO", "Se ejecuto correctamente el SET y el espacio usado en la instancia es %d", entries_used);
}

void client_server_execute_client(int fd_server) {
	fd_coordinador = fd_server;

	main_mock();
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";

	total_entries = (argc < 2) ? 16 : atoi(argv[1]);
	entry_size = (argc < 3) ? 4 : atoi(argv[2]);
	client_name = (argc < 4) ? "Instancia 1" : argv[3];

	client_server_run();
}

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"

int total_entries, entry_size;

void client_server_execute_server(int fd_client) {
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
}

void client_server_execute_client(int fd_server) {
	fd_coordinador = fd_server;

	storage_setup_init(0, 0);

	coordinator_api_handshake(client_name, storage_setup);

	storage_setup_destroy();
}

int main(int argc, char* argv[]) {
	server_name = "Coordinador";

	total_entries = (argc < 2) ? 16 : atoi(argv[1]);
	entry_size = (argc < 3) ? 4 : atoi(argv[2]);
	client_name = (argc < 4) ? "Instancia 1" : argv[3];

	client_server_run();
}

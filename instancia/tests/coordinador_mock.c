	#include <commons/string.h>

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "coordinador_mock.h"

void coordinador_mock_handshake(int fd_client, size_t total_entries, size_t entry_size) {
	storage_setup_t setup = {.total_entries = total_entries, .entry_size = entry_size};

	char* received_name;

	receive_handshake(fd_client);

	send_confirmation(fd_client, true);

	chunk_recv_variable(fd_client, (void**) &received_name);

	messenger_show("INFO", "Se recibio una solicitud de handshake de la instancia %s", received_name);

	messenger_show("INFO", "Se va a enviar el tamano del storage de %d entradas de tamano %d a %s", setup.total_entries, setup.entry_size, received_name);

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &setup.entry_size, sizeof(size_t));

	chunk_add(chunk, &setup.total_entries, sizeof(size_t));

	chunk_send_and_destroy(fd_client, chunk);

	free(received_name);
}

void coordinador_mock_set_request(int fd_client, char* key, char* value) {
	messenger_show("INFO", "Pedido de SET %s, %s", key, value);

	chunk_t* chunk = chunk_create();

	request_coordinador header = PROTOCOL_CI_SET;

	chunk_add(chunk, &header, sizeof(header));
	chunk_add_variable(chunk, key, string_length(key) + 1);
	chunk_add_variable(chunk, value, string_length(value) + 1);

	chunk_send_and_destroy(fd_client, chunk);
}

int coordinador_mock_set_response(int fd_client) {
	int header, status_received, entries_used_received;

	chunk_recv(fd_client, &header, sizeof(status_received));
	chunk_recv(fd_client, &status_received, sizeof(status_received));
	chunk_recv(fd_client, &entries_used_received, sizeof(entries_used_received));

	messenger_show("INFO", "Se ejecuto un SET con un status %d y en la instancia se usan %d entradas", status_received, entries_used_received);

	return status_received;
}

void coordinador_mock_store_request(int fd_client, char* key) {
	messenger_show("INFO", "Pedido de STORE %s", key);

	chunk_t* chunk = chunk_create();

	request_coordinador header = PROTOCOL_CI_STORE;

	chunk_add(chunk, &header, sizeof(header));
	chunk_add_variable(chunk, key, string_length(key) + 1);

	chunk_send_and_destroy(fd_client, chunk);
}

int coordinador_mock_store_response(int fd_client) {
	int header, status_received;

	chunk_recv(fd_client, &header, sizeof(status_received));
	chunk_recv(fd_client, &status_received, sizeof(status_received));

	messenger_show("INFO", "Se ejecuto un STORE con un status %d", status_received);

	return status_received;
}

void coordinador_mock_kill(int fd_client) {
	messenger_show("INFO", "Desconectando a la Instancia");

	chunk_t* chunk = chunk_create();

	request_coordinador header = PROTOCOL_CI_KILL;

	chunk_add(chunk, &header, sizeof(header));

	chunk_send_and_destroy(fd_client, chunk);
}

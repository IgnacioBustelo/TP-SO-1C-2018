#include <commons/string.h>

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "coordinador_mock.h"

void coordinador_mock_handshake(int fd_client, size_t total_entries, size_t entry_size) {
	chunk_t* chunk = chunk_create();

	bool is_new_instance = true;

	request_coordinador header = PROTOCOL_CI_HANDSHAKE_CONFIRMATION;

	storage_setup_t setup = {.total_entries = total_entries, .entry_size = entry_size};

	char *received_name, *existing_instance_name = "Esquivel";

	receive_handshake(fd_client);

	chunk_recv_variable(fd_client, (void**) &received_name);

	messenger_show("INFO", "Se recibio una solicitud de handshake de la instancia %s", received_name);

	is_new_instance = !string_equals_ignore_case(received_name, existing_instance_name);

	chunk_add(chunk, &header, sizeof(header));

	chunk_add(chunk, &is_new_instance, sizeof(is_new_instance));

	if(is_new_instance) {
		messenger_show("INFO", "Se va a enviar el tamano del storage de %d entradas de tamano %d a %s", setup.total_entries, setup.entry_size, received_name);

		chunk_add(chunk, &setup.entry_size, sizeof(size_t));

		chunk_add(chunk, &setup.total_entries, sizeof(size_t));
	}

	else {
		messenger_show("ERROR", "La instancia %s ya existe en el sistema", received_name);
	}

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

void coordinador_mock_set_response(int fd_client) {
	int header, status_received, entries_used_received;

	chunk_recv(fd_client, &header, sizeof(status_received));
	chunk_recv(fd_client, &status_received, sizeof(status_received));
	chunk_recv(fd_client, &entries_used_received, sizeof(entries_used_received));

	messenger_show("INFO", "Se recibio la operacion %d con un status %d y en la instancia se usan %d entradas", header, status_received, entries_used_received);
}

void coordinador_mock_kill(int fd_client) {
	messenger_show("INFO", "Desconectando a la Instancia");

	chunk_t* chunk = chunk_create();

	request_coordinador header = PROTOCOL_CI_KILL;

	chunk_add(chunk, &header, sizeof(header));

	chunk_send_and_destroy(fd_client, chunk);
}

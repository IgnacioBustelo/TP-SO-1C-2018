#include <commons/string.h>

#include "../../libs/chunker.h"
#include "../../libs/messenger.h"
#include "../../libs/mocks/client_server.h"
#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "../globals.h"
#include "coordinador_mock.h"

static void _chunk_add_key(chunk_t* chunk, void* content) {
	messenger_show("INFO", "Se debe recuperar la clave %s", (char*) content);

	chunk_add_variable(chunk, (char*) content, string_length((char*) content) + 1);
}

void coordinador_mock_handshake_base(int fd_client, bool* is_accepted) {
	receive_handshake(fd_client);

	messenger_show((*is_accepted) ? "INFO" : "ERROR", "Se recibio una solictiud de una instancia que va a ser %s", (*is_accepted) ? "aceptada" : "rechazada");

	send_confirmation(fd_client, *is_accepted);
}

void coordinador_mock_handshake_receive_name(int fd_client, char** received_name) {
	chunk_recv_variable(fd_client, (void**) received_name);

	messenger_show("INFO", "Se recibio una solicitud de handshake de la instancia %s", *received_name);

}

void coordinador_mock_handshake_send_config(int fd_client, char* received_name, size_t total_entries, size_t entry_size, t_list* recoverable_keys) {
	storage_setup_t setup = {.total_entries = total_entries, .entry_size = entry_size};

	messenger_show("INFO", "Se va a enviar el tamano del storage de %d entradas de tamano %d a %s", setup.total_entries, setup.entry_size, received_name);

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &setup.entry_size, sizeof(size_t));

	chunk_add(chunk, &setup.total_entries, sizeof(size_t));

	chunk_add_list(chunk, recoverable_keys, (void*) _chunk_add_key);

	chunk_send_and_destroy(fd_client, chunk);
}

void coordinador_mock_set_request(int fd_client, bool is_new, char* key, char* value) {
	request_coordinador header = PROTOCOL_CI_SET;

	messenger_show("INFO", "Pedido de %sSET %s, %s a traves del mensaje %s", (is_new) ? "nuevo " : "", key, value, C_HEADER(header));

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));
	chunk_add(chunk, &is_new, sizeof(is_new));
	chunk_add_variable(chunk, key, string_length(key) + 1);
	chunk_add_variable(chunk, value, string_length(value) + 1);

	chunk_send_and_destroy(fd_client, chunk);
}

int coordinador_mock_set_response(int fd_client) {
	int header, status_received, entries_used_received;

	chunk_recv(fd_client, &header, sizeof(status_received));
	chunk_recv(fd_client, &status_received, sizeof(status_received));
	chunk_recv(fd_client, &entries_used_received, sizeof(entries_used_received));

	messenger_show("INFO", "Se ejecuto un SET con un status %s y en la instancia se usan %d entradas", CI_STATUS(status_received), entries_used_received);

	return status_received;
}

void coordinador_mock_store_request(int fd_client, char* key) {
	request_coordinador header = PROTOCOL_CI_STORE;

	messenger_show("INFO", "Pedido de STORE %s a traves del mensaje %s", key, C_HEADER(header));

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));
	chunk_add_variable(chunk, key, string_length(key) + 1);

	chunk_send_and_destroy(fd_client, chunk);
}

int coordinador_mock_store_response(int fd_client) {
	int header, status_received;

	chunk_recv(fd_client, &header, sizeof(status_received));
	chunk_recv(fd_client, &status_received, sizeof(status_received));

	messenger_show("INFO", "Se ejecuto un STORE con un status %s", CI_STATUS(status_received));

	return status_received;
}

void coordinador_mock_status_request(int fd_client, char* key) {
	request_coordinador header = PROTOCOL_CI_REQUEST_VALUE;

	messenger_show("INFO", "Pedido del valor de la clave %s a traves del mensaje %s", key, C_HEADER(header));

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));
	chunk_add_variable(chunk, key, string_length(key) + 1);
	chunk_send_and_destroy(fd_client, chunk);
}

char* coordinador_mock_status_response(int fd_client, char* key) {
	char* value;
	int header, status_received;

	chunk_recv(fd_client, &header, sizeof(header));
	chunk_recv(fd_client, &status_received, sizeof(status_received));
	chunk_recv_variable(fd_client, (void**) &value);

	messenger_show("INFO", "Se obtuvo el valor '%s' de la clave '%s' con estado '%s'", value, key, CI_STATUS(status_received));

	return value;
}

void coordinador_mock_check_request(int fd_client) {
	request_coordinador header = PROTOCOL_CI_IS_ALIVE;

	messenger_show("INFO", "Chequeo de conexion de la Instancia con el mensaje %s", C_HEADER(header));

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));

	chunk_send_and_destroy(fd_client, chunk);
}

int	coordinador_mock_check_response(int fd_client) {
	int header;

	int bytes_received = chunk_recv(fd_client, &header, sizeof(header));

	if(header == PROTOCOL_IC_CONFIRM_CONNECTION) {
		messenger_show("INFO", "La Instancia sigue conectada tras recibir el mensaje %s", I_HEADER(header));
	}

	else if (bytes_received == -1) {
		messenger_show("ERROR", "Hubo error en el envio de datos");
	}

	else {
		messenger_show("WARNING", "Mensaje inseperado");
	}

	return bytes_received;
}

void coordinador_mock_kill(int fd_client) {
	request_coordinador header = PROTOCOL_CI_KILL;

	messenger_show("INFO", "Desconectando a la Instancia a traves del mensaje %s", C_HEADER(header));

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));

	chunk_send_and_destroy(fd_client, chunk);
}

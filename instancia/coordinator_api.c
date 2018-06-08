#include <commons/string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../libs/chunker.h"
#include "../libs/conector.h"
#include "../libs/messenger.h"
#include "coordinator_api.h"
#include "globals.h"

void coordinator_api_connect(char* host, int port) {
	if(fd_coordinador == 0) {
		fd_coordinador = connect_to_server(host, port);
	}

	// TODO: Manejar error cuando no encuentra al Coordinador
}

void coordinator_api_handshake(char* instance_name, storage_setup_t* setup){
	chunk_t* chunk;

	request_coordinador header;

	bool is_confirmed = false;

	messenger_show("INFO", "Enviada la solicitud de handshake con el Coordinador");

	send_handshake(fd_coordinador, INSTANCE);

	messenger_show("INFO", "Se prepara el envio del nombre de la instancia (%s)", instance_name);

	chunk = chunk_create();

	chunk_add_variable(chunk, instance_name, string_length(instance_name) + 1);

	chunk_send_and_destroy(fd_coordinador, chunk);

	chunk_recv(fd_coordinador, &header, sizeof(header));

	chunk_recv(fd_coordinador, &is_confirmed, sizeof(is_confirmed));

	if(is_confirmed) {
		chunk_recv(fd_coordinador, &setup->entry_size, sizeof(size_t));

		chunk_recv(fd_coordinador, &setup->total_entries, sizeof(size_t));

		messenger_show("INFO", "Se asigno una dimension de %d entradas de tamano %d para el Storage", setup->total_entries, setup->entry_size);
	}

	else {
		messenger_show("ERROR", "El Coordinador no acepto a la Instancia");
	}
}

request_coordinador coordinator_api_receive_header() {
	request_coordinador header;

	chunk_recv(fd_coordinador, &header, sizeof(request_coordinador));

	return header;
}

key_value_t* coordinator_api_receive_set() {
	char *key, *value;

	chunk_recv_variable(fd_coordinador, (void**) &key);
	chunk_recv_variable(fd_coordinador, (void**) &value);

	key_value_t* key_value = key_value_create(key, value);

	messenger_show("INFO", "Se recibio la clave %s con valor %s de tamano %d", key_value->key, key_value->value, key_value->size);

	free(key);
	free(value);

	return key_value;
}

char* coordinator_api_receive_store() {
	char *key;

	chunk_recv_variable(fd_coordinador, (void**) &key);

	return key;
}

void coordinator_api_notify_header(request_instancia header) {
	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));

	chunk_send_and_destroy(fd_coordinador, chunk);
}

void coordinator_api_notify_status(request_instancia header, int status) {
	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));
	chunk_add(chunk, &status, sizeof(status));

	chunk_send_and_destroy(fd_coordinador, chunk);
}

void coordinator_api_notify_set(int status, size_t entries_used) {
	request_instancia header = PROTOCOL_IC_NOTIFY_STATUS;

	messenger_show("INFO", "Notificar al Coordinador el status %d y la cantidad de entradas usadas, que es %d", status, entries_used);

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));
	chunk_add(chunk, &status, sizeof(status));
	chunk_add(chunk, &entries_used, sizeof(entries_used));

	chunk_send_and_destroy(fd_coordinador, chunk);
}

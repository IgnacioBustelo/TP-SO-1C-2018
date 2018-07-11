#include <commons/string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../libs/chunker.h"
#include "../libs/conector.h"
#include "../libs/messenger.h"
#include "coordinator_api.h"
#include "globals.h"

#define API_CHECK_FULL(BYTES, ERROR_VALUE, MSG)	\
	if(BYTES == ERROR_VALUE) {					\
		messenger_show("ERROR", MSG);			\
												\
		return API_ERROR;						\
	}											\

#define API_R_CHECK(MSG) API_CHECK_FULL(status, -1, MSG)

static void* _recv_recoverable_key(int fd, int* bytes_received) {
	void* key;

	*bytes_received = chunk_recv_variable(fd, &key);

	return key;
}

int coordinator_api_connect(char* host, int port) {
	if(fd_coordinador == 0) {
		fd_coordinador = connect_to_server(host, port);
		API_CHECK_FULL(fd_coordinador, -1, "Fallo en la conexion con el Coordinador");
	}

	messenger_show("DEBUG", "Asignacion del socket %d para comunicarse con el Coordinador", fd_coordinador);

	return API_SUCCESS;
}

int	coordinator_api_handshake_base(bool* is_confirmed) {
	int status;

	messenger_show("DEBUG", "Envio de solicitud de handshake al Coordinador");

	status = send_handshake(fd_coordinador, INSTANCE);
	API_CHECK_FULL(status, 0, "Fallo enviando el tipo de proceso al Coordinador");

	status = receive_confirmation(fd_coordinador, is_confirmed);
	API_CHECK_FULL(status, 0, "Fallo recibiendo confirmacion del Handshake");

	messenger_show("DEBUG", "Recibo de %s del Coordinador", *is_confirmed ? "confirmacion" : "rechazo");

	return API_SUCCESS;
}

int	coordinator_api_handshake_send_name(char* instance_name) {
	int status;

	chunk_t* chunk = chunk_create();

	chunk_add_variable(chunk, instance_name, string_length(instance_name) + 1);

	messenger_show("DEBUG", "Envio del nombre de la Instancia, que es %s", instance_name);

	status = chunk_send_and_destroy(fd_coordinador, chunk);
	API_R_CHECK("Fallo enviando el nombre de la Instancia");

	return API_SUCCESS;
}

int	coordinator_api_handshake_receive_config(storage_setup_t* setup, t_list** recoverable_keys) {
	int status;

	status = chunk_recv(fd_coordinador, &setup->entry_size, sizeof(size_t));
	API_R_CHECK("Fallo recibiendo el tamaño de entradas");

	status = chunk_recv(fd_coordinador, &setup->total_entries, sizeof(size_t));
	API_R_CHECK("Fallo recibiendo el total de entradas");

	status = chunk_recv_list(fd_coordinador, recoverable_keys, _recv_recoverable_key);
	API_R_CHECK("Fallo recibiendo la lista de claves a recuperar");

	messenger_show("DEBUG", "Asignacion de %d entradas de tamano %d para el Storage", setup->total_entries, setup->entry_size);

	if(list_is_empty(*recoverable_keys)) {
		messenger_show("DEBUG", "No es necesario recuperar claves");
	}

	else {
		char* key_list = messenger_list_to_string(*recoverable_keys);

		messenger_show("DEBUG", "Se deben recuperar %d claves: [%s]", list_size(*recoverable_keys), key_list);

		free(key_list);
	}

	return API_SUCCESS;
}

int coordinator_api_receive_header(request_coordinador*	header) {
	int status;

	status = chunk_recv(fd_coordinador, header, sizeof(*header));
	API_R_CHECK("Fallo recibiendo la peticion del Coordinador");

	messenger_show("DEBUG", "Recibo de header %s", C_HEADER(*header));

	return API_SUCCESS;
}

int	coordinator_api_receive_set(bool* is_new, key_value_t** key_value) {
	int status;

	char *key, *value;

	status = chunk_recv(fd_coordinador, is_new, sizeof(*is_new));
	API_R_CHECK("Fallo recibiendo estado de la clave");

	status = chunk_recv_variable(fd_coordinador, (void**) &key);
	API_R_CHECK("Fallo recibiendo la clave");

	status = chunk_recv_variable(fd_coordinador, (void**) &value);
	API_R_CHECK("Fallo recibiendo el valor de la clave");

	*key_value = key_value_create(key, value);

	messenger_show("DEBUG", "Recibo de la %sclave %s con valor %s de tamano %d", (*is_new) ? "nueva " : "", (*key_value)->key, (*key_value)->value, (*key_value)->size);

	free(key);
	free(value);

	return API_SUCCESS;
}

int coordinator_api_receive_key(char** key) {
	int status;

	status = chunk_recv_variable(fd_coordinador, (void**) key);
	API_R_CHECK("Fallo recibiendo la clave");

	messenger_show("DEBUG", "Recibo de la clave %s", *key);

	return API_SUCCESS;
}

int coordinator_api_notify_header(request_instancia header) {
	int status;

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));

	messenger_show("DEBUG", "Envio del header %s al Coordinador", I_HEADER(header));

	status = chunk_send_and_destroy(fd_coordinador, chunk);
	API_R_CHECK("Fallo enviando header");

	return API_SUCCESS;
}

int coordinator_api_notify_status(request_instancia header, int op_result) {
	int status;

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));
	chunk_add(chunk, &op_result, sizeof(op_result));

	messenger_show("DEBUG", "Envio del header %s con el resultado de operacion %s al Coordinador", I_HEADER(header), CI_STATUS(op_result));

	status = chunk_send_and_destroy(fd_coordinador, chunk);
	API_R_CHECK("Fallo enviando el header con el resultado de la operacion");

	return API_SUCCESS;
}

int coordinator_api_notify_set(size_t entries_used, int op_result) {
	int status;

	request_instancia header = PROTOCOL_IC_NOTIFY_STATUS;

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));
	chunk_add(chunk, &op_result, sizeof(op_result));
	chunk_add(chunk, &entries_used, sizeof(entries_used));

	messenger_show("DEBUG", "Envio del header %s con el resultado de operacion %s y la cantidad de entradas usadas (%d) al Coordinador", I_HEADER(header), CI_STATUS(op_result), entries_used);

	status = chunk_send_and_destroy(fd_coordinador, chunk);
	API_R_CHECK("Fallo enviando el header con el resultado de la operacion y la cantidad de entradas usadas");

	return API_SUCCESS;
}

int	coordinator_api_notify_key_value(key_value_t* key_value, int op_result) {
	int status;

	request_instancia header = PROTOCOL_IC_RETRIEVE_VALUE;

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &header, sizeof(header));
	chunk_add(chunk, &op_result, sizeof(op_result));
	chunk_add_variable(chunk, key_value->value, key_value->size + 1);

	messenger_show("DEBUG", "Envio del header %s con el resultado de operacion %s y el valor '%s' de la clave '%s'", I_HEADER(header), CI_STATUS(op_result), key_value->value, key_value->key);

	status = chunk_send_and_destroy(fd_coordinador, chunk);
	API_R_CHECK("Fallo enviando el header con el estado de la clave y su valor");

	return API_SUCCESS;
}

int coordinator_api_disconnect() {
	int status;

	messenger_show("DEBUG", "Desconexion de la Instancia");

	status = close(fd_coordinador);
	API_R_CHECK("Fallo cerrando el socket");

	return API_SUCCESS;
}

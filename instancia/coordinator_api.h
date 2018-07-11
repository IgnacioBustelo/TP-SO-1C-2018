#ifndef INSTANCIA_COORDINATOR_API_H_
#define INSTANCIA_COORDINATOR_API_H_

#include <commons/collections/list.h>
#include <stdbool.h>

#include "../protocolo/protocolo_coordinador_instancia.h"

#include "globals.h"

// Estructuras de datos

typedef enum {
	API_SUCCESS,
	API_ERROR,
} coordinator_api_status;

// Atributos

int fd_coordinador;

// Interfaz

int	coordinator_api_connect(char* host, int port);

int	coordinator_api_handshake_base(bool* is_confirmed);

int	coordinator_api_handshake_send_name(char* instance_name);

int	coordinator_api_handshake_receive_config(storage_setup_t* setup, t_list** recoverable_keys);

int	coordinator_api_receive_header(request_coordinador*	header);

int	coordinator_api_receive_set(bool* is_new, key_value_t** key_value);

int	coordinator_api_receive_key(char** key);

int	coordinator_api_notify_header(request_instancia header);

int coordinator_api_notify_status(request_instancia header, int op_result);

int	coordinator_api_notify_set(size_t entries_used, int op_result);

int	coordinator_api_notify_key_value(key_value_t* key_value, int op_result);

int coordinator_api_disconnect();

#endif

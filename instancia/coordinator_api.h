#ifndef INSTANCIA_COORDINATOR_API_H_
#define INSTANCIA_COORDINATOR_API_H_

#include <commons/collections/list.h>
#include <stdbool.h>

#include "../protocolo/protocolo_coordinador_instancia.h"
#include "globals.h"

// Estructuras de datos

typedef enum {
	API_RECV_ERROR,
	API_HANDSHAKE_SUCCESS,
	API_HANDSHAKE_ERROR,
	API_SET_SUCCESS
} coordinator_api_status;

// Atributos

int fd_coordinador;

// Interfaz

int					coordinator_api_connect(char* host, int port);

int					coordinator_api_handshake(char* instance_name, storage_setup_t* setup, t_list** recoverable_keys);

request_coordinador	coordinator_api_receive_header();

key_value_t*		coordinator_api_receive_set(bool* is_new);

char*				coordinator_api_receive_key();

void				coordinator_api_notify_header(request_instancia header);

void				coordinator_api_notify_status(request_instancia header, int status);

void				coordinator_api_notify_set(int status, size_t entries_used);

void				coordinator_api_notify_key_value(int status, key_value_t* key_value);

void				coordinator_api_disconnect();

#endif

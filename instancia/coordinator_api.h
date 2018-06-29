#ifndef INSTANCIA_COORDINATOR_API_H_
#define INSTANCIA_COORDINATOR_API_H_

#include "../protocolo/protocolo_coordinador_instancia.h"
#include "globals.h"

// Estructuras de datos

typedef enum {
	SET_SUCCESS,
	HANDSHAKE_SUCCESS,
	HANDSHAKE_ERROR
} coordinator_api_status;

// Atributos

int fd_coordinador;

// Interfaz

void				coordinator_api_connect(char* host, int port);

int					coordinator_api_handshake(char* instance_name, storage_setup_t* setup);

request_coordinador	coordinator_api_receive_header();

key_value_t*		coordinator_api_receive_set();

char*				coordinator_api_receive_store();

void				coordinator_api_notify_header(request_instancia header);

void				coordinator_api_notify_status(request_instancia header, int status);

void				coordinator_api_notify_set(int status, size_t entries_used);

#endif

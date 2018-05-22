#ifndef INSTANCIA_COORDINATOR_API_H_
#define INSTANCIA_COORDINATOR_API_H_

#include "../protocolo/protocolo_coordinador_instancia.h"
#include "globals.h"

// Estructuras de datos

typedef enum {
	SET_SUCCESS,
	HANDSHAKE_SUCCES
} coordinator_api_status;

// Atributos

int fd_instancia;

int fd_coordinador;

// Interfaz

void				coordinator_api_connect(char* host, int port);

void				coordinator_api_handshake(char* instance_name); // Lo hace Nico

request_coordinador	coordinator_api_receive_header();

key_value_t*		coordinator_api_receive_set(); // Hablado con Fer

void				coordinator_api_notify_status(int status); // Lo hace Santi

#endif

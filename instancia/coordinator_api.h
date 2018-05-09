#ifndef INSTANCIA_COORDINATOR_API_H_
#define INSTANCIA_COORDINATOR_API_H_

#include "globals.h"

// Estructuras de datos

typedef enum {
	SET_SUCCESS
} coordinator_api_status;

// Atributos

int fd_instancia;

int fd_coordinador;

// Interfaz

void			coordinator_api_handshake(); // Lo hace Nico

key_value_t*	coordinator_api_receive_set(); // Hablado con Fer

void			coordinator_api_notify_status(int status); // Lo hace Santi

#endif

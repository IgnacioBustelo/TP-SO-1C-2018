#ifndef INSTANCIA_COORDINATOR_API_H_
#define INSTANCIA_COORDINATOR_API_H_

#include "../protocolo/protocolo_coordinador_instancia.h"
#include "instancia.h"

int coordinator_handshake(); // Lo hace Nico

void coordinator_api_request(); // NUEVO: A discutir entre Nico y Santi

key_value_t* coordinator_receive_set(); // Hablado con Fer

int coordinator_notify_status(int status); // Lo hace Santi

#endif /* INSTANCIA_COORDINATOR_API_H_ */

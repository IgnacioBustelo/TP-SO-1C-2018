#ifndef INSTANCIA_COORDINATOR_INTERFACE_H_
#define INSTANCIA_COORDINATOR_INTERFACE_H_

#include "instancia.h"

int coordinator_handshake(int fd_coordinator); // Lo hace Nico

key_value_t* coordinator_receive_set(int fd_coordinator); // Hablado con Fer

int coordinator_notify_status(int fd_coordinator, int status); // Lo hace Santi

#endif /* INSTANCIA_COORDINATOR_INTERFACE_H_ */

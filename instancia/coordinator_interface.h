#ifndef INSTANCIA_COORDINATOR_INTERFACE_H_
#define INSTANCIA_COORDINATOR_INTERFACE_H_

#include "entry_table.h"

int coordinator_handshake(int fd_coordinator); //N

key_value_t* coordinator_receive_set(int fd_coordinator); //SN en linea

int coordinator_notify_status(int fd_coordinator, int status);//S Hablar con fer

#endif /* INSTANCIA_COORDINATOR_INTERFACE_H_ */

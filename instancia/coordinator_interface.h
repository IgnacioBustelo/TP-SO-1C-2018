#ifndef INSTANCIA_COORDINATOR_INTERFACE_H_
#define INSTANCIA_COORDINATOR_INTERFACE_H_

#include "entry_table.h"

int coordinator_handshake(int fd_coordinator);

key_value_t* coordinator_receive_set(int fd_coordinator);

int coordinator_notify_status(int fd_coordinator, int status);

#endif /* INSTANCIA_COORDINATOR_INTERFACE_H_ */

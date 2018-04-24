#ifndef LIBS_HANDSHAKER_C_
#define LIBS_HANDSHAKER_C_

#include "conector.h"

int handshake_client(int server_fd, char* server_name, enum process_type type, t_log* logger);

int handshake_server(); // TODO: Implementar handshake para los servidores

#endif

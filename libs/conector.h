#ifndef CONECTOR_H_
#define CONECTOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <arpa/inet.h>

enum process_type {
	SCHEDULER, ESI, INSTANCE
};

/**
 * Retorna el FD.
 */
int connect_to_server(char *host, int port);

#define NO_FD_ERROR		-1
#define BIND_ERROR		-2
#define LISTEN_ERROR	-3

/**
 * Retorna el socket de escucha.
 * Si hubo error, devuelve un valor negativo.
 */
int init_listener(int listen_port, int max_conn);

#endif /* CONECTOR_H_ */

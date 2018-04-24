#include <sys/socket.h>
#include <stdlib.h>

#include "deserializador.h"

#define RECV_DATA_ERROR		-1
#define RECV_SIZE_ERROR		-2
#define RECV_MALLOC_ERROR	-3

int recv_package(int fd, void* receiver, size_t size) {
	int status = recv(fd, receiver, size, MSG_WAITALL);

	if(status == -1) {
		return RECV_DATA_ERROR;
	}

	else {
		return EXIT_SUCCESS;
	}
}

int recv_package_variable(int fd, void** receiver) {
	int status;
	size_t package_size;

	status = recv_package(fd, &package_size, sizeof(package_size));

	if(status == RECV_DATA_ERROR) {
		return RECV_SIZE_ERROR;
	}

	*receiver = malloc(package_size);

	if(receiver == NULL) {
		return RECV_MALLOC_ERROR;
	}

	status = recv_package(fd, *receiver, package_size);

	return status;
}

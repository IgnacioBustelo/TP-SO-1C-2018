#include "serializador.h"

#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

package_t* create_package(size_t size) {
	package_t* new_package = malloc(sizeof(package_t));

	new_package->load = malloc(size);
	new_package->size = size;

	return new_package;
}

void add_content(package_t* package, void* content, size_t content_size) {
	memcpy(package->load, content, content_size);

	package->load += content_size;
}

void add_content_variable(package_t* package, void* content, size_t content_size) {
	add_content(package, &content_size, sizeof(size_t));

	add_content(package, content, content_size);
}

void* build_package(package_t* package) {
	package->load -= package->size;

	return package->load;
}

int	send_serialized_package(int fd, void* serialized_package, size_t package_size) {
	int bytes_sent = send(fd, serialized_package, package_size, 0);

	if(bytes_sent < 0) {
		return SEND_ERROR;
	}

	else if(bytes_sent < package_size) {
		return send_serialized_package(fd, serialized_package + bytes_sent, package_size - bytes_sent);
	}

	else {
		return EXIT_SUCCESS;
	}
}

package_t* receive_package(int socket_sender) {
	package_t *package = malloc(sizeof(package_t));

	int ret = recv(socket_sender, &(package->size), sizeof(package->size), MSG_WAITALL);
	if (ret == -1) {
		free(package);
		return NULL;
	}

	package->load = malloc(package->size);

	ret = recv(socket_sender, package->load, package->size, MSG_WAITALL);
	if (ret == -1) {
		free(package->load);
		free(package);
		return NULL;
	}

	return package;
}

void destroy_package(package_t* package) {
	free(package->load);
	free(package);
}

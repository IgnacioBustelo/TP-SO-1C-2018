#include "coordinator_api.h"

#include <stdlib.h>
#include <unistd.h>

#include "../libs/deserializador.h"
#include "../libs/serializador.h"
#include "instancia.h"

void coordinator_api_request() {
	request_coordinador header;

	recv_package(fd_coordinador, &header, sizeof(request_coordinador));

	switch(header) {
		case PROTOCOL_CI_SET: instance_set(); break;

		default: break;
	}
}

key_value_t* coordinator_receive_set() {
	key_value_t* key_value = malloc(sizeof(key_value_t));

	recv_package_variable(fd_coordinador, (void**) &key_value->key);
	recv_package(fd_coordinador, &key_value->size, sizeof(size_t));
	recv_package(fd_coordinador, &key_value->value, key_value->size);

	return key_value;
}

int coordinator_notify_status(int status) {
	return send(fd_coordinador, &status, sizeof(int), 0);
}

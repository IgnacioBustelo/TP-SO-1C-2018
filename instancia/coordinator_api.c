#include <commons/string.h>
#include <stdlib.h>
#include <unistd.h>

#include "../protocolo/protocolo_coordinador_instancia.h"
#include "../libs/conector.h"
#include "../libs/deserializador.h"
#include "../libs/serializador.h"
#include "../libs/serializador_v2.h"
#include "coordinator_api.h"
#include "globals.h"


void coordinator_api_handshake(char* instance_name){
	chunk_t *data;
	void* message;


	send_handshake(fd_coordinador,INSTANCE);



	data = chunk_create();
	chunk_add_variable(data, instance_name, strlen(instance_name)+1);
	message = chunk_build(data);
	send_serialized_package(fd_coordinador,message,data->current_size);
	chunk_destroy(data);
	free(message);

	if (receive_handshake(fd_coordinador)){

		recv_package(fd_coordinador,&storage_setup.entry_size,sizeof(size_t));
		recv_package(fd_coordinador,&storage_setup.total_entries,sizeof(size_t));
	}
}
key_value_t* coordinator_api_receive_set() {
	char *key, *value;

	recv_package_variable(fd_coordinador, (void**) &key);
	recv_package_variable(fd_coordinador, (void**) &value);

	key_value_t* key_value = key_value_create(key, value);

	free(key);
	free(value);

	return key_value;
}

void coordinator_api_notify_status(int status) {
	chunk_t* chunk = chunk_create();
	request_instancia header = PROTOCOL_IC_NOTIFY_STATUS;

	chunk_add(chunk, &header, sizeof(header));
	chunk_add(chunk, &status, sizeof(status));

	void* status_message = chunk_build(chunk);

	send_serialized_package(fd_coordinador, &status_message, chunk->current_size);

	free(status_message);
	chunk_destroy(chunk);
}

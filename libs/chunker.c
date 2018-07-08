#include "chunker.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

#include "messenger.h"

chunk_t* chunk_create() {
	chunk_t* chunk = malloc(sizeof(chunk_t));

	chunk->bytes = malloc(0);
	chunk->current_size = 0;

	messenger_show("DEBUG", "Se inicio un paquete para serializar");

	return chunk;
}

void chunk_add(chunk_t* chunk, void* content, size_t content_size) {
	chunk->bytes = realloc(chunk->bytes, chunk->current_size + content_size);

	memcpy(chunk->bytes + chunk->current_size, content, content_size);

	chunk->current_size += content_size;

	messenger_show("DEBUG", "Se añadio un contenido de tamano %d al paquete para serializar", content_size);
}

void chunk_add_variable(chunk_t* chunk, void* content, size_t content_size) {
	chunk_add(chunk, &content_size, sizeof(content_size));

	chunk_add(chunk, content, content_size);
}

void chunk_add_list(chunk_t* chunk, t_list* list, void(*packager)(chunk_t* chunk, void* content)) {
	size_t size = list_size(list);

	chunk_add(chunk, &size, sizeof(size_t));

	int i;
	for(i = 0; i < size; i++) {
		packager(chunk, list_get(list, i));
	}
}

void chunk_show(chunk_t* chunk) {
	char* chunk_string = messenger_bytes_to_string(chunk->bytes, chunk->current_size);

	messenger_show("DEBUG", "El paquete contiene la cadena %s serializada", chunk_string);

	free(chunk_string);
}

void* chunk_build(chunk_t* chunk) {
	void* serialized_chunk = malloc(chunk->current_size);

	memcpy(serialized_chunk, chunk->bytes, chunk->current_size);

	messenger_show("DEBUG", "Se genero un cacho de memoria serializado de tamanio %d", chunk->current_size);

	return serialized_chunk;
}

void chunk_destroy(chunk_t* chunk) {
	messenger_show("DEBUG", "Se libero un paquete de tamano %d para serializar", chunk->current_size);

	free(chunk->bytes);
	free(chunk);
}

int chunk_send(int fd, void* serialized_chunk, size_t chunk_size) {
	int bytes_sent;

	messenger_show("DEBUG", "Se enviaran %d bytes", chunk_size);

	bytes_sent = send(fd, serialized_chunk, chunk_size, MSG_NOSIGNAL);

	if(bytes_sent == -1) {
		messenger_show("ERROR", "Error enviando datos");

		return bytes_sent;
	}

	else if(bytes_sent < chunk_size) {
		messenger_show("DEBUG", "Faltan enviar %d bytes", bytes_sent);

		return chunk_send(fd, serialized_chunk + bytes_sent, chunk_size - bytes_sent);
	}

	else {
		return bytes_sent;
	}
}

int chunk_send_and_destroy(int fd, chunk_t* chunk) {
	int bytes_sent;
	size_t size = chunk->current_size;
	void* serialized_chunk = chunk_build(chunk);

	chunk_destroy(chunk);

	bytes_sent = chunk_send(fd, serialized_chunk, size);

	free(serialized_chunk);

	return bytes_sent;
}

int chunk_recv(int fd, void* receiver, size_t size) {
	int bytes_received;

	do {

		bytes_received = recv(fd, receiver, size, MSG_WAITALL | MSG_NOSIGNAL);

	}

	while (bytes_received == -1 && errno == EINTR);

	if(bytes_received != -1) {
		messenger_show("DEBUG", "Se recibio un cacho serializado de memoria de tamano %d, despues de haber recibido %d bytes", size, bytes_received);
	}

	else {
		messenger_show("ERROR", "Fallo recibiendo un paquete de tamanio %d", size);
	}

	return bytes_received;
}

int chunk_recv_variable(int fd, void** receiver) {
	int bytes_size_received, bytes_data_received;
	size_t size;

	bytes_size_received = chunk_recv(fd, &size, sizeof(size));

	if(bytes_size_received == -1) {
		messenger_show("ERROR", "Fallo recibiendo el tamanio del paquete de tamanio variable");

		return -1;
	}

	*receiver = malloc(size);

	bytes_data_received = chunk_recv(fd, *receiver, size);

	if(bytes_data_received == -1) {
		messenger_show("ERROR", "Fallo recibiendo el paquete de tamanio variable");

		return bytes_data_received;
	}

	messenger_show("DEBUG", "Se recibio un cacho variable serializado de memoria de tamano %d", size);

	return bytes_size_received + bytes_data_received;
}

int chunk_recv_list(int fd, t_list** receiver, void*(*unpackager)(int fd, int* bytes_received)) {
	int bytes_received;
	size_t size;

	*receiver = list_create();

	bytes_received = chunk_recv(fd, &size, sizeof(size));

	if(bytes_received == -1) {
		messenger_show("ERROR", "Fallo recibiendo la cantidad de nodos de la lista");

		return bytes_received;
	}

	if(size < 1) {
		messenger_show("WARNING", "No se recibio ningun elemento");

		return bytes_received;
	}

	else {
		messenger_show("INFO", "Se va a recibir una lista de %d elemento/s", size);
	}

	int i;
	for(i = 0; i < size; i++) {
		int current_received_bytes;

		void* received_data = unpackager(fd, &current_received_bytes);

		if(current_received_bytes == -1) {
			messenger_show("ERROR", "Fallo recibiendo la cantidad de nodos de la lista");

			return current_received_bytes;
		}

		bytes_received += current_received_bytes;

		list_add(*receiver, received_data);
	}

	messenger_show("DEBUG", "Se recibio una lista de %d elemento/s que ocupa %d bytes", size, bytes_received);

	return bytes_received;
}

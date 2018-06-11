#include "chunker.h"

#include <stdlib.h>
#include <string.h>
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

void chunk_add_list(chunk_t* chunk, t_list* list, void(*packager)(chunk_t*, void*)) {
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

void chunk_send(int fd, void* serialized_chunk, size_t chunk_size) {
	messenger_show("DEBUG", "Se enviaron %d bytes", chunk_size);

	int bytes_sent = send(fd, serialized_chunk, chunk_size, 0);

	if(bytes_sent < chunk_size) {
		chunk_send(fd, serialized_chunk + bytes_sent, chunk_size - bytes_sent);
	}
}

void chunk_send_and_destroy(int fd, chunk_t* chunk) {
	size_t size = chunk->current_size;
	void* serialized_chunk = chunk_build(chunk);

	chunk_destroy(chunk);

	chunk_send(fd, serialized_chunk, size);

	free(serialized_chunk);
}

void chunk_recv(int fd, void* receiver, size_t size) {
	recv(fd, receiver, size, MSG_WAITALL);

	messenger_show("DEBUG", "Se recibio un cacho serializado de memoria de tamano %d", size);
}

void chunk_recv_variable(int fd, void** receiver) {
	size_t size;

	recv(fd, &size, sizeof(size), MSG_WAITALL);

	*receiver = malloc(size);

	recv(fd, *receiver, size, MSG_WAITALL);

	messenger_show("DEBUG", "Se recibio un cacho variable serializado de memoria de tamano %d", size);
}

void chunk_recv_list(int fd, t_list** receiver, void*(*unpackager)(int)) {
	size_t size;

	*receiver = list_create();

	chunk_recv(fd, &size, sizeof(size));

	int i;
	for(i = 0; i < size; i++) {
		list_add(*receiver, unpackager(fd));
	}
}

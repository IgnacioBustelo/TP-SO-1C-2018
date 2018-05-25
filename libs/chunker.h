#ifndef LIBS_CHUNKER_H_
#define LIBS_CHUNKER_H_

#include <stdlib.h>

// TODO: Manejar errores

typedef struct {
	void* bytes;
	size_t current_size;
} chunk_t;

chunk_t*	chunk_create();

void		chunk_add(chunk_t* chunk, void* content, size_t content_size);

void		chunk_add_variable(chunk_t* chunk, void* content, size_t content_size);

void		chunk_show(chunk_t* chunk);

void*		chunk_build(chunk_t* chunk);

void		chunk_destroy(chunk_t* chunk);

void		chunk_send(int fd, void* serialized_chunk, size_t chunk_size);

void		chunk_send_and_destroy(int fd, chunk_t* chunk);

void		chunk_recv(int fd, void* receiver, size_t size);

void		chunk_recv_variable(int fd, void** receiver);

#endif

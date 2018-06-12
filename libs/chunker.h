#ifndef LIBS_CHUNKER_H_
#define LIBS_CHUNKER_H_

#include <commons/collections/list.h>
#include <stdlib.h>

// TODO: Manejar errores

typedef struct {
	void* bytes;
	size_t current_size;
} chunk_t;

chunk_t*	chunk_create();

void		chunk_add(chunk_t* chunk, void* content, size_t content_size);

void		chunk_add_variable(chunk_t* chunk, void* content, size_t content_size);

void		chunk_add_list(chunk_t* chunk, t_list* list, void(*packager)(chunk_t* chunk, void* content));

void		chunk_show(chunk_t* chunk);

void*		chunk_build(chunk_t* chunk);

void		chunk_destroy(chunk_t* chunk);

int			chunk_send(int fd, void* serialized_chunk, size_t chunk_size);

int			chunk_send_and_destroy(int fd, chunk_t* chunk);

int			chunk_recv(int fd, void* receiver, size_t size);

int			chunk_recv_variable(int fd, void** receiver);

int			chunk_recv_list(int fd, t_list** receiver, void*(*unpackager)(int fd, int* bytes_received));

#endif

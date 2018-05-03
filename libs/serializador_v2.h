#ifndef LIBS_SERIALIZADOR_V2_H_
#define LIBS_SERIALIZADOR_V2_H_

#include <stdlib.h>

typedef struct {
	void* bytes;
	size_t current_size;
} chunk_t;

chunk_t*	chunk_create();

void		chunk_add(chunk_t* chunk, void* content, size_t content_size);

void		chunk_add_variable(chunk_t* chunk, void* content, size_t content_size);

void*		chunk_build(chunk_t* chunk);

void		chunk_destroy(chunk_t* chunk);

#endif

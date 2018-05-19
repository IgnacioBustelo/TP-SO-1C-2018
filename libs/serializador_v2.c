#include <stdlib.h>
#include <string.h>

#include "serializador_v2.h"

chunk_t* chunk_create() {
	chunk_t* chunk = malloc(sizeof(chunk_t));

	chunk->bytes = malloc(0);
	chunk->current_size = 0;

	return chunk;
}

void chunk_add(chunk_t* chunk, void* content, size_t content_size) {
	chunk->bytes = realloc(chunk->bytes, chunk->current_size + content_size);

	memcpy(chunk->bytes + chunk->current_size, content, content_size);

	chunk->current_size += content_size;
}

void chunk_add_variable(chunk_t* chunk, void* content, size_t content_size) {
	chunk_add(chunk, &content_size, sizeof(content_size));

	chunk_add(chunk, content, content_size);
}

void* chunk_build(chunk_t* chunk) {
	void* serialized_chunk = malloc(chunk->current_size);

	memcpy(serialized_chunk, chunk->bytes, chunk->current_size);

	return serialized_chunk;
}

void chunk_destroy(chunk_t* chunk) {
	free(chunk->bytes);
	free(chunk);
}

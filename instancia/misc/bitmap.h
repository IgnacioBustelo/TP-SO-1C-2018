#ifndef INSTANCIA_MISC_BITMAP_H_
#define INSTANCIA_MISC_BITMAP_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	size_t	size;
	char**	bit_vector;
} bitmap_t;

bitmap_t* bitmap;

void	bitmap_init(int size);

int		bitmap_next_index(int size);

bool	bitmap_is_insertable(int from_index, size_t size);

void	bitmap_insert(int index, char* key, size_t size);

int		bitmap_update(int index, char* key, size_t new_size, size_t old_size);

void	bitmap_delete(int index, size_t size);

void	bitmap_show();

void	bitmap_destroy();

#endif

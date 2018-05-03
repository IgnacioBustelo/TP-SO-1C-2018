#ifndef LIBS_SERIALIZADOR_V2_H_
#define LIBS_SERIALIZADOR_V2_H_

#include <stdlib.h>

typedef struct {
	void* load;
	size_t current_size;
} package_t;

package_t* package_create();

void package_add(package_t* package, void* content, size_t content_size);

void package_add_variable(package_t* package, void* content, size_t content_size);

void* package_build(package_t* package);

void package_destroy(package_t* package);

#endif

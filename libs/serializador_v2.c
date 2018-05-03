#include <stdlib.h>

#include "serializador_v2.h"

package_t* package_create() {
	package_t* package = malloc(sizeof(package_t));

	package->current_size = 0;

	return package;
}

void package_add(package_t* package, void* content, size_t content_size) {
	package->load = realloc(package->load, package->current_size + content_size);

	memcpy(package->load + package->current_size, content, content_size);

	package->current_size += content_size;
}

void package_add_variable(package_t* package, void* content, size_t content_size) {
	package_add(package, &content_size, sizeof(content_size));

	package_add(package, content, content_size);
}

void* package_build(package_t* package) {
	void* serialized_package = malloc(package->current_size);

	memcpy(serialized_package, package->load, package->current_size);

	return serialized_package;
}

void package_destroy(package_t* package) {
	free(package->load);
	free(package);
}

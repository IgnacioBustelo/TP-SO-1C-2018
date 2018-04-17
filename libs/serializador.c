#include "serializador.h"

#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>


package_t* create_package(size_t size) {
	package_t* new_package = malloc(sizeof(package_t));

	new_package->load = malloc(size);
	new_package->size = size;

	return new_package;
}

void add_content(package_t* package, void* content, size_t content_size) {
	memcpy(package->load, content, content_size);

	package->load += content_size;
}

void add_content_variable(package_t* package, void* content, size_t content_size) {
	add_content(package, &content_size, sizeof(size_t));

	add_content(package, content, content_size);
}

void* build_package(package_t* package) {
	package->load -= package->size;

	return package->load;
}

void destroy_package(package_t* package) {
	free(package->load);
	free(package);
}

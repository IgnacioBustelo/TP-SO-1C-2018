#ifndef LIBS_SERIALIZADOR_H_
#define LIBS_SERIALIZADOR_H_

typedef struct {
	void*	load;
	size_t	size;
} package_t;

package_t*	create_package(size_t size);

void		add_content(package_t* package, void* content, size_t content_size);

void		add_content_variable(package_t* package, void* content, size_t content_size);

void		add_content_list(package_t* package, t_list* list, size_t (*size_calculator)(void*));

void*		build_package(package_t* package);

void		destroy_package(package_t* package);

#endif

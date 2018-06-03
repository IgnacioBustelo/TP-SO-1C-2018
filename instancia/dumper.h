#ifndef INSTANCIA_DUMPER_H_
#define INSTANCIA_DUMPER_H_

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <stdlib.h>

typedef struct {
	char*			mount_point;
	t_dictionary*	file_dictionary;
} dumper_t;

dumper_t* dumper;

void	dumper_init(char* mount_point);

int		dumper_create_key_value(char* key);

void	dumper_remove_key_value(char* key);

void	dumper_store(int fd_key, void* data, size_t size);

t_list*	dumper_recover();

void	dumper_show();

void	dumper_destroy();

#endif

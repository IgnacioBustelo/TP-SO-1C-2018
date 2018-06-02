#ifndef INSTANCIA_DUMPER_H_
#define INSTANCIA_DUMPER_H_

#include <stdlib.h>

char*	dumper_mount_point;

void	dumper_init(char* mount_point);

int		dumper_create_key_value(char* key);

void	dumper_remove_key_value(char* key, int key_fd);

void	dumper_store(int fd_key, void* data, size_t size);

#endif

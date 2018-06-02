#include <commons/string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "../libs/messenger.h"
#include "dumper.h"
#include "globals.h"

static char* get_key_value_file_name(char* key) {
	char* file_name = string_from_format("%s%s.txt", dumper_mount_point, key);

	return file_name;
}

void dumper_init(char* mount_point) {
	struct stat storage_stat = {0};

	if (stat(mount_point, &storage_stat) == -1) {
	    mkdir(mount_point, S_IRWXU);
	}

	dumper_mount_point = mount_point;
}

int dumper_create_key_value(char* key) {
	char* file_name = get_key_value_file_name(key);

	int key_fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);

	free(file_name);

	return key_fd;
}

void dumper_remove_key_value(char* key, int key_fd) {
	char* file_name = get_key_value_file_name(key);

	close(key_fd);

	remove(file_name);

	free(file_name);
}

void dumper_store(int fd_key, void* data, size_t size) {
	ftruncate(fd_key, size);

	void* mapped_memory = mmap(NULL, size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd_key, 0);

	memcpy(mapped_memory, data, size);

	munmap(mapped_memory, size);

	messenger_show("INFO", "Se ejecuto un STORE que ocupa %d bytes", size);
}

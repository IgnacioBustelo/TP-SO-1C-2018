#include <commons/string.h>
#include <dirent.h>
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

static size_t get_stored_value(int key_fd, void** value) {
	struct stat key_fd_stat;

	fstat(key_fd, &key_fd_stat);

	size_t value_size = key_fd_stat.st_size;

	*value = malloc(value_size);

	read(key_fd, *value, value_size);

	lseek(key_fd, 0, SEEK_SET);

	return value_size;
}

static char* get_key_value_file_name(char* key) {
	char* file_name = string_from_format("%s%s.txt", dumper->mount_point, key);

	return file_name;
}

static char* get_file_name_key_value(char* file_name) {
	char** file_name_splitted = string_split(file_name, ".");

	char* key = string_duplicate(file_name_splitted[0]);

	string_iterate_lines(file_name_splitted, (void*) free);

	free(file_name_splitted);

	return key;
}

static key_value_t* recover_key_value(char* file_name) {
	void* data;

	char* key = get_file_name_key_value(file_name);

	int fd_key = dumper_create_key_value(key);

	size_t data_size = get_stored_value(fd_key, &data);

	char* data_string = messenger_bytes_to_string(data, data_size);

	key_value_t* recovered_key_value = key_value_create(key, data_string);

	free(data);

	free(data_string);

	free(key);

	return recovered_key_value;
}

int dumper_create_key_value(char* key) {
	char* file_name = get_key_value_file_name(key);

	int key_fd = open(file_name, O_RDWR | O_CREAT, S_IRWXU);

	free(file_name);

	dictionary_put(dumper->file_dictionary, key, (void*) key_fd);

	return key_fd;
}

void dumper_remove_key_value(char* key) {
	int key_fd = (int) dictionary_remove(dumper->file_dictionary, key);

	close(key_fd);

	char* file_name = get_key_value_file_name(key);

	remove(file_name);

	free(file_name);
}

void dumper_init(char* mount_point) {
	dumper = malloc(sizeof(dumper_t));

	struct stat storage_stat = {0};

	if (stat(mount_point, &storage_stat) == -1) {
	    mkdir(mount_point, S_IRWXU);
	}

	dumper->mount_point = mount_point;
	dumper->file_dictionary = dictionary_create();
}

void dumper_store(char* key, void* data, size_t size) {
	int fd_key = (int) dictionary_get(dumper->file_dictionary, key);

	ftruncate(fd_key, size);

	void* mapped_memory = mmap(NULL, size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd_key, 0);

	memcpy(mapped_memory, data, size);

	munmap(mapped_memory, size);
}

t_list* dumper_recover() {
	t_list* recovered_key_values = list_create();

	DIR* mount_directory = opendir(dumper->mount_point);

	struct dirent* current_file;

	while((current_file = readdir(mount_directory)) != NULL) {
		if (!string_equals_ignore_case(current_file->d_name, ".") && !string_equals_ignore_case(current_file->d_name, "..")) {
			key_value_t* recovered_key_value = recover_key_value(current_file->d_name);

			list_add(recovered_key_values, (void*) recovered_key_value);
		}
	}

	free(current_file);

	closedir(mount_directory);

	return recovered_key_values;
}

void dumper_show() {
	void fd_show(char* key, void* value) {
		void* content;

		size_t content_size = get_stored_value((int) value, &content);

		char* content_string = messenger_bytes_to_string(content, content_size);

		messenger_show("DEBUG", "FD: %d - Clave: %s - Valor: %s - Tamanio = %d", (int) value, key, content_string, content_size + 1);

		free(content_string);

		free(content);
	}

	messenger_show("DEBUG", "Listado de claves persistidas");

	dictionary_iterator(dumper->file_dictionary, (void*) fd_show);
}

void dumper_destroy() {
	dictionary_destroy_and_destroy_elements(dumper->file_dictionary, (void*) close);

	free(dumper);
}

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
	if(!string_contains(file_name, ".txt")) {
		return string_new();
	}

	int length = string_length(file_name) - 4; // Le resto 4 Por la cadena '.txt'

	return string_substring_until(file_name, length);
}

static key_value_t* recover_key_value(char* key) {
	void* data;

	int fd_key = dumper_create_key_value(key);

	size_t data_size = get_stored_value(fd_key, &data);

	char* data_string = messenger_bytes_to_string(data, data_size);

	key_value_t* recovered_key_value = key_value_create(key, data_string);

	free(data);

	free(data_string);

	return recovered_key_value;
}

static bool is_recoverable(char* recovered_key, t_list* keys) {
	bool key_equals(void* key) {
		return string_equals_ignore_case(recovered_key, (char*) key);
	}

	bool is_valid_file_name = !string_equals_ignore_case(recovered_key, ".") && !string_equals_ignore_case(recovered_key, "..");

	bool is_in_keys = list_any_satisfy(keys, (void*) key_equals);

	return is_valid_file_name && is_in_keys;
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

int dumper_init(char* mount_point) {
	dumper = malloc(sizeof(dumper_t));

	struct stat storage_stat;

	int directory_exists = stat(mount_point, &storage_stat);

	if (directory_exists == -1) {
	    mkdir(mount_point, S_IRWXU);
	}

	dumper->mount_point = mount_point;
	dumper->file_dictionary = dictionary_create();

	return directory_exists;
}

void dumper_store(char* key, void* data, size_t size) {
	int fd_key = dictionary_has_key(dumper->file_dictionary, key) ? (int) dictionary_get(dumper->file_dictionary, key) : dumper_create_key_value(key);

	ftruncate(fd_key, size);

	// TODO: Podria poner este mmap en el dictionary e inicializarlo cuando se añade la clave, y munmapearlo cuando se elimina

	void* mapped_memory = mmap(NULL, size, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd_key, 0);

	memcpy(mapped_memory, data, size);

	munmap(mapped_memory, size);
}

t_list* dumper_recover(t_list* keys) {
	t_list* recovered_key_values = list_create();

	DIR* mount_directory = opendir(dumper->mount_point);

	struct dirent* current_file;

	while((current_file = readdir(mount_directory)) != NULL) {
		char* recovered_key = get_file_name_key_value(current_file->d_name);

		if (is_recoverable(recovered_key, keys)) {
			key_value_t* recovered_key_value = recover_key_value(recovered_key);

			list_add(recovered_key_values, (void*) recovered_key_value);
		}

		free(recovered_key);
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

		messenger_show("DEBUG", "FD: %d - Clave: %s - Valor: %s - Tamanio = %d", (int) value, key, content_string, content_size);

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

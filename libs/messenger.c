#define _GNU_SOURCE

#include <commons/string.h>
#include <string.h>
#include <stdio.h>

#include "messenger.h"

void messenger_show(char* log_level, const char* format,...) {
	va_list args;

	va_start(args, format);

	char* message;

	vasprintf(&message, format, args);

	messenger_show_method(log_level, message);

	free(message);

	va_end(args);
}

char* messenger_bytes_to_string(void* data, size_t size) {
	char* chunk_string = malloc(size + 1);

	memset(chunk_string, 0, size + 1);

	memcpy(chunk_string, data, size);

	return chunk_string;
}

char* messenger_list_to_string(t_list* string_list) {
	char* csv_string_list = string_new();

	int i, size = list_size(string_list) - 1;
	for(i = 0; i < size; i++) {
		string_append(&csv_string_list, (char*) list_get(string_list, i));
		string_append(&csv_string_list, ", ");
	}

	string_append(&csv_string_list, (char*) list_get(string_list, i));

	return csv_string_list;
}

int messenger_longest_string_length(int max_number) {
	char* max_number_string = string_itoa(max_number);

	int longest_length = string_length(max_number_string);

	free(max_number_string);

	return longest_length;
}

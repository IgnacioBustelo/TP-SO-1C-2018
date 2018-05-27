#include <commons/string.h>
#include <string.h>

#include "messenger.h"

void messenger_show(char* log_level, const char* format,...) {
	va_list args;

	va_start(args, format);

	char* message = string_from_vformat(format, args);

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

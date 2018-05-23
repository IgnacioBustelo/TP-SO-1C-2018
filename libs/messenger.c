#include <commons/string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "messenger.h"

void messenger_show(char* log_level, const char* format,...) {
	va_list args;

	va_start(args, format);

	char* message = string_from_vformat(format, args);

	messenger_show_method(log_level, message);

	free(message);

	va_end(args);
}

#include "printfer.h"

#include <commons/string.h>
#include <commons/temporal.h>
#include <stdio.h>
#include <stdlib.h>

void messenger_show_method(char* log_level, char* message) {
	char* current_time = temporal_get_string_time();

	printf("%-.13s - %-.7s \t | %s\n", current_time, log_level, message);

	free(current_time);
}

// TODO: Ver referencia de messenger_log
void messenger_log(char* message, char* level) {
	char* current_time = temporal_get_string_time();

	printf("%-.13s-[%-.7s] %s\n", current_time, level, message);

	free(current_time);
}

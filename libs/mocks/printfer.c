#include "printfer.h"

#include <commons/string.h>
#include <commons/temporal.h>
#include <stdio.h>
#include <stdlib.h>


void messenger_log(char* message, char* level) {
	char* current_time = temporal_get_string_time();

	printf("%-.13s-[%-.7s] %s\n", current_time, level, message);

	free(current_time);
}

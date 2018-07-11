#include "color.h"
#include "printfer.h"

#include <commons/string.h>
#include <commons/process.h>
#include <commons/temporal.h>
#include <stdio.h>
#include <stdlib.h>

bool can_trace = false, can_debug = false;

void messenger_init(char* logger_route, char* process_name, char* log_level) {
	printf(COLOR_RESET);
}

void messenger_show_method(char* log_level, char* message) {
	if(!can_trace && string_equals_ignore_case(log_level, "TRACE")) {
		return;
	}

	else if(!can_debug && string_equals_ignore_case(log_level, "DEBUG")) {
		return;
	}

	char* current_time = temporal_get_string_time();

	char* color = (string_equals_ignore_case(log_level, "ERROR")) ? COLOR_RED : (string_equals_ignore_case(log_level, "WARNING")) ? COLOR_YELLOW : color_get();

	printf("PID <%d> %s%-12s - %-7s | %s%s\n", process_getpid(), color, current_time, log_level, message, COLOR_RESET);

	free(current_time);
}

void printfer_set_levels(bool allow_trace, bool allow_debug) {
	can_trace = allow_trace;
	can_debug = allow_debug;
}

void printfer_next_line() {
	printf("\n");
}

void messenger_destroy() {
	printf(COLOR_RESET);
}

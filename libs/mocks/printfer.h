#ifndef LIBS_MOCKS_PRINTFER_H_
#define LIBS_MOCKS_PRINTFER_H_

#include <stdbool.h>

#include "color.h"
#include "../messenger.h"

void printfer_set_levels(bool allow_trace, bool allow_debug);

void printfer_next_line();

#endif

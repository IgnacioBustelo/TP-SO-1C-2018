#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>

#include "../../libs/mocks/printfer.h"
#include "../storage.h"

static int required_entries(int size) {
	int required_entries = size/storage->frame_size;

	return size % storage->frame_size == 0 ? required_entries : ++required_entries;
}

int main(int argc, char* argv[]) {
	int i, next_entry = 0, current_entry = 0;

	storage_init((argc < 2) ? 10 : (size_t) atoi(argv[1]), (argc < 3) ? 5 : (size_t) atoi(argv[2]));

	for(i = 3; next_entry < storage->frames && i < argc; i++) {

		current_entry = next_entry;

		next_entry = current_entry + required_entries(string_length(argv[i]) + 1);

		if(next_entry < storage->frames) {
			storage_set(current_entry, argv[i], string_length(argv[i] + 1));
		}

	}

	storage_show();

	storage_destroy();
}

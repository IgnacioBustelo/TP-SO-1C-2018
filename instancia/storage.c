#include <commons/string.h>

#include "storage.h"

int storage_init() {
	storage = dictionary_create();

	if(storage == NULL) {
		return STRG_INIT_CREATE_ERROR;
	}

	int i;
	for(i = 0; i < total_entries; i++) {
		char* key = string_itoa(i);

		dictionary_put(storage, key, NULL);

		free(key);
	}

	return STRG_INIT_SUCCESS;
}

#include "entry_table.h"
#include "instancia.h"
#include "storage.h"

void instance_set(key_value_t* key_value) {
	int next_entry = entry_table_next_entry(key_value);

	storage_set(next_entry, key_value);

	entry_table_update(next_entry, key_value);
}

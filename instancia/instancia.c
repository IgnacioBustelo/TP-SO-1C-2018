#include "entry_table.h"
#include "instancia.h"
#include "storage.h"

void instance_set(key_value_t* key_value) {
	int next_entry = entry_table_next_entry(key_value);

	storage_set(next_entry, key_value->value, key_value->size);

	entry_table_update(next_entry, key_value);
}

int instance_entries_used() {
	int entries_used = 0;

	// TODO: Logica para obtener cantidad de entradas usadas.

	return entries_used;
}

#include "entry_table.h"
#include "instancia.h"
#include "storage.h"

void instance_set(key_value_t* key_value) {
	int next_entry = entry_table_next_entry(key_value);
	// Para no olvidarme, llegue a la conclusion de que storage deberia calcular si un key value entra. Mas que nada por que este sabe donde puede guardarse
	//el proximo dato ( dir fisica), en cambio entry table no sabe cuales son los bloques libre... si no que sabe si hay boques libres o no!
	//ACTUALIZACION: Dije cualquiera
	storage_set(next_entry, key_value);
	entry_table_update(next_entry, key_value);
}

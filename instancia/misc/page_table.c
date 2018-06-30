#include "../../libs/messenger.h"

#include "page_table.h"

void page_table_init() {
	page_table = dictionary_create();
}

entry_t* page_table_select(char* key) {
	entry_t* selected_entry = dictionary_get(page_table, key);

	return entry_new(selected_entry->id, selected_entry->size);
}

void page_table_insert(char* key, entry_t* new_entry) {
	dictionary_put(page_table, key, new_entry);
}

void page_table_update(char* key, entry_t* updated_entry) {
	page_table_delete(key);

	page_table_insert(key, updated_entry);
}

void page_table_delete(char* key) {
	dictionary_remove_and_destroy(page_table, key, (void*) entry_destroy);
}

bool page_table_exists_key(char* key) {
	return dictionary_has_key(page_table, key);
}

void page_table_show() {
	messenger_show("INFO", "Lista de entradas alojadas en la Instancia");

	dictionary_iterator(page_table, (void*) entry_show);
}

void page_table_destroy() {
	dictionary_destroy_and_destroy_elements(page_table, (void*) entry_destroy);
}

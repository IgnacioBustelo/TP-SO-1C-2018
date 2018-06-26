#include "../compactation.h"

int main() {
	storage_setup_init(5, 4);

	entry_table_init();

	storage_init(5, 4);

	key_value_t *key_1 = key_value_create("A", "X"), *key_2 = key_value_create("B", "Y");

	entry_table_insert(0, key_1);

	storage_set(0, key_1->value, key_1->size);

	entry_table_insert(3, key_2);

	storage_set(3, key_2->value, key_2->size);

	entry_table_print_table();

	storage_show();

	compact();

	entry_table_print_table();

	storage_show();

	key_value_destroy(key_1);

	key_value_destroy(key_2);

	storage_destroy();
}


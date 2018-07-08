#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../../libs/mocks/color.h"
#include "../../libs/mocks/printfer.h"
#include "../../libs/messenger.h"
#include "../event_handler.h"
#include "../dumper.h"
#include "../globals.h"

static t_list* key_generator() {
	t_list* generated_keys = list_create();

	static int n = 0, i;

	for(i = 0; i < 5; i++) {
		char *key = string_from_format("clave_%d", n), *string_value = string_from_format("valor_%d", (n + 1)*n);

		key_value_t* key_value = key_value_create(key, string_value);

		list_add(generated_keys, key_value);

		n++;
	}

	return generated_keys;
}

static void dump(void* node) {
	key_value_t* key_value = (key_value_t*) node;

	dumper_store(key_value->key, key_value->value, key_value-> size);

	messenger_show("INFO", "Dump de la clave %s%s%s con valor %s%s%s", COLOR_BLUE, key_value->key, COLOR_RESET, COLOR_MAGENTA, key_value->value, COLOR_RESET);
}

int main(int argc, char* argv[]) {
	int dump_interval = argc == 1 ? 5 : atoi(argv[1]), time_passed = 0;

	event_handler_init();

	dumper_init("dumped_keys/");

	messenger_show("INFO", "Prueba de dump cada %d segundo/s", dump_interval);

	do
	{
		sleep(dump_interval);

		time_passed += dump_interval;

		if(instance_is_alive) {
			messenger_show("INFO", "Ejecucion de dump en el directorio %s en el segundo %d", dumper->mount_point, time_passed);

			t_list* generated_keys = key_generator();

			list_iterate(generated_keys, (void*) dump);

			list_destroy_and_destroy_elements(generated_keys, (void*) key_value_destroy);
		}

	} while(instance_is_alive);

	printfer_next_line();

	dumper_destroy();

	messenger_show("INFO", "La instancia fue desconectada tras %d segundo/s", time_passed);
}

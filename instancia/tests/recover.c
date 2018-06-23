#include <stdlib.h>

#include "../../libs/messenger.h"
#include "../dumper.h"
#include "../globals.h"

#define MOUNT_POINT "backup/"

void key_value_show(void* node) {
	key_value_t* key_value = (key_value_t*) node;

	messenger_show("INFO", "Clave: %s - Valor: %s - Tamanio: %d", key_value->key, key_value->value, key_value->size);
}

int main(int argc, char* argv[]) {
	t_list* keys = list_create();

	if(argc == 1) {
		messenger_show("ERROR", "Inserte las claves que quiere recuperar del directorio %s", MOUNT_POINT);

		list_destroy(keys);

		exit(EXIT_FAILURE);
	}

	int i;
	for(i = 1; i < argc; i++) {
		list_add(keys, (char*) argv[i]);
	}

	int directory_exists = dumper_init(MOUNT_POINT);

	if(directory_exists == -1) {
		messenger_show("ERROR", "No existia el directorio '%s'. Se acaba de crear para que ingrese archivos ahi para hacer la prueba", MOUNT_POINT);

		list_destroy(keys);

		dumper_destroy();

		exit(EXIT_FAILURE);
	}

	t_list* recovered_key_values = dumper_recover(keys);

	if(list_is_empty(recovered_key_values)) {
		messenger_show("WARNING","No se recupero ninguna clave", dumper->mount_point);

		list_destroy(keys);

		list_destroy(recovered_key_values);

		dumper_destroy();

		exit(EXIT_FAILURE);
	}

	messenger_show("INFO", "Mostrando lista de claves recuperadas de %s", dumper->mount_point);

	list_iterate(recovered_key_values, (void*) key_value_show);

	messenger_show("INFO", "Mostrando diccionario de claves con archivos abiertos");

	dumper_show();

	list_destroy_and_destroy_elements(recovered_key_values, (void*) key_value_destroy);

	list_destroy(keys);

	dumper_destroy();
}

#include <stdlib.h>

#include "../../libs/messenger.h"
#include "../dumper.h"
#include "../globals.h"

void key_value_show(void* node) {
	key_value_t* key_value = (key_value_t*) node;

	messenger_show("INFO", "Clave: %s \t Valor: %s \t Tamanio: %d", key_value->key, key_value->value, key_value->size);
}

int main(int argc, char* argv[]) {
	dumper_init("backup/");

	t_list* recovered_key_values = dumper_recover();

	if(list_is_empty(recovered_key_values)) {
		messenger_show("INFO", "Inserte claves con sus valores en el directorio %s para hacer la prueba", dumper->mount_point);
	}

	else {
		messenger_show("INFO", "Mostrando lista de claves recuperadas de %s", dumper->mount_point);

		list_iterate(recovered_key_values, (void*) key_value_show);

		messenger_show("INFO", "Mostrando diccionario de claves con archivos abiertos");

		dumper_show();
	}

	list_destroy_and_destroy_elements(recovered_key_values, (void*) key_value_destroy);

	dumper_destroy();
}

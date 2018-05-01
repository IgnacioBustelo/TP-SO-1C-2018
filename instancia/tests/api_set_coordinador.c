#include <stdlib.h>

#include "../../libs/conector.h"
#include "../../libs/serializador.h"
#include "../coordinator_api.h"
#include "utils.h"

#define HOST "127.0.0.1"
#define PORT 8080

int main(int argc, char* argv[]) {
	printf("Coordinador: Inicio\n");

	key_value_t* key_value = (argc < 3) ? key_value_create("A", "aaaaskccbb") : key_value_create(argv[1], argv[2]);
	package_t* package = create_package(sizeof(size_t) * 2 + string_length(key_value->key) + string_length(key_value->value) + 2);

	printf("Coordinador: Clave a enviar: ");
	print_key_value(key_value);

	add_content_variable(package, key_value->key, string_length(key_value->key) + 1);
	add_content_variable(package, key_value->value, string_length(key_value->value) + 1);

	void* message = build_package(package);

	int server_fd = connect_to_server(HOST, PORT);

	printf("Coordinador: Conectado a la Instancia\n");

	send_serialized_package(server_fd, message, package->size);

	destroy_package(package);
	key_value_destroy(key_value);
	free(message);
	close(server_fd);
}

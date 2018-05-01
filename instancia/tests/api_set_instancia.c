#include <stdlib.h>

#include "../libs/conector.h"
#include "../libs/serializador.h"
#include "../coordinator_api.h"

// Utilidades

static void print_key_value(key_value_t* key_value) {
	printf("Clave: %s\t Tamanio: %d\t Valor: %s\n", key_value->key, key_value->size, key_value->value);
}

static void instancia() {
	int server_fd = init_listener(8080, 1);

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;

	int client_fd = accept(server_fd, (struct sockaddr *) &client_info, &addrlen);

	key_value_t* key_value = coordinator_receive_set();

	print_key_value(key_value);


}

static void coordinador_mock(int argc, char* argv[]) {
	if(argc < 3) {

	}


}

int main(int argc, char* argv[]) {
	int pid = fork();

	if(pid > 0) {
		instancia();
	}

	else if(pid == 0) {
		coordinador_mock(argc, argv);
	}

	else {
		printf("Fallo craendo procesos\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

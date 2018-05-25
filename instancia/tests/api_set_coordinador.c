#include <commons/string.h>
#include <stdlib.h>

#include "../../libs/chunker.h"
#include "../../libs/conector.h"

#define HOST "127.0.0.1"
#define PORT 8080

int main(int argc, char* argv[]) {
	printf("Coordinador: Inicio\n");

	char *key, *value;

	key = (argc < 3) ? string_duplicate("A") : string_duplicate(argv[1]);
	value = (argc < 3) ? string_duplicate("aaaaskccbb") : string_duplicate(argv[2]);

	chunk_t* chunk = chunk_create();

	printf("Coordinador: Clave a enviar: [%s; %s]\n", key, value);

	chunk_add_variable(chunk, key, string_length(key) + 1);
	chunk_add_variable(chunk, value, string_length(value) + 1);

	void* message = chunk_build(chunk);

	int server_fd = connect_to_server(HOST, PORT);

	printf("Coordinador: Conectado a la Instancia\n");

	send(server_fd, message, chunk->current_size, 0);

	printf("Coordinador: Clave enviada\n");

	free(key);
	free(value);
	free(message);
	chunk_destroy(chunk);
	close(server_fd);
}

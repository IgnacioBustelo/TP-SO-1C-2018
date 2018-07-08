#include <string.h>

#include "../mocks/client_server.h"
#include "../chunker.h"
#include "../messenger.h"

size_t size;

void client_server_execute_server(int fd_client) {
	void* data = malloc(size);

	memset(data, 1, size);

	chunk_t* chunk = chunk_create();

	chunk_add(chunk, data, size);

	chunk_send_and_destroy(fd_client, chunk);

	free(data);
}

void client_server_execute_client(int fd_server) {
	void* data = malloc(size);

	int bytes_received = chunk_recv(fd_server, data, size);

	if(bytes_received == size) {
		messenger_show("INFO", "Se recibio la informacion correctamente");
	}

	else {
		messenger_show("ERROR", "Se recibieron %d bytes cuando se esperaban %d", bytes_received, size);
	}

	free(data);
}

int main(int argc, char* argv[]) {
	size = (argc == 1) ? 1000 : atoi(argv[1]);

	client_name = "Cliente";
	server_name = "Servidor";

	client_server_run();
}

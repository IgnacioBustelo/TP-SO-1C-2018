#include "../coordinator_api.h"
#include "../../libs/conector.h"

#define HOST "127.0.0.1"
#define PORT 8080

int main(int argc, char* argv[]) {
	char* instance_name = (argv[1] == NULL) ? strdup("Instancia 1"): argv[1];

	printf("Instancia: Se levanta con nombre %s\n", instance_name);

	int fd_coordinador = connect_to_server(HOST, PORT);

	coordinator_api_handshake(instance_name);

	printf("Instancia: Se realizo el Handshake correctamente");

	free(instance_name);
	close(fd_coordinador);
}

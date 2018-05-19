#include "../../libs/conector.h"
#include "../../libs/deserializador.h"

#define PORT 8080

int main(void) {
	printf("Coordinador: Inicio\n");

	char* instance_name;

	int fd_coordinador = init_listener(PORT, 1);

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;

	printf("Coordinador: Esperando una Instancia\n");

	int fd_instancia = accept(fd_coordinador, (struct sockaddr *) &client_info, &addrlen);

	receive_handshake(fd_instancia);

	send_confirmation(fd_instancia, true);

	recv_package_variable(fd_instancia, (void**) &instance_name);

	printf("Coordinador: Instancia %s aceptada\n", instance_name);

	free(instance_name);
	close(fd_instancia);
	close(fd_coordinador);
}

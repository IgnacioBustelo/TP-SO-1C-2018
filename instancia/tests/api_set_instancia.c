#include <stdlib.h>

#include "../../libs/conector.h"
#include "../coordinator_api.h"
#include "utils.h"

#define PORT 8080

int main(void) {
	printf("Instancia: Inicio\n");

	fd_instancia = init_listener(PORT, 1);

	struct sockaddr_in client_info;
	socklen_t addrlen = sizeof client_info;

	printf("Instancia: Esperando al Coordinador\n");

	fd_coordinador = accept(fd_instancia, (struct sockaddr *) &client_info, &addrlen);

	printf("Instancia: Coordinador aceptado\n");

	key_value_t* key_value = coordinator_receive_set();

	printf("Instancia: Clave obtenida: ");
	print_key_value(key_value);

	key_value_destroy(key_value);

	close(fd_coordinador);
	close(fd_instancia);
}

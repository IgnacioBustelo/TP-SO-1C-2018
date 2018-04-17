#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libs/conector.h"

int main(void) {
	init_log();

	init_config();

	//TODO obtener la ip y el puerto del coordinador del archivo de configuracion

	char* ip_coordinador; // TODO Falta asignar valor
	int port_coordinator; // TODO Falta asignar valor

//Conexion al coordinador

	int coordinator_fd = connect_to_server(ip_coordinador, port_coordinator);
		if (send_handshake(coordinator_fd, ESI) != 1) {
			log_error(logger, "Failure in send_handshake");
			close(coordinator_fd);
		}

		bool confirmation;
	int received = receive_confirmation(coordinator_fd, &confirmation);
		if (!received || !confirmation) {
			log_error(logger, "Failure in confirmation reception");
			close(coordinator_fd);
		}

	return EXIT_SUCCESS;
}

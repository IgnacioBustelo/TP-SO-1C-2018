#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libs/conector.h"
#include "config.h"
#include "instancia.h"



// Global variables

t_log* logger;

setup_t setup;

int coordinador_fd;


// Main thread

int main(void) {

	logger = init_log();

	setup = init_config(logger);

	coordinador_fd = connect_to_server(setup.coordinator_ip, setup.coordinator_port);

	// TODO: Cambiar por handshake_client()

		if (send_handshake(coordinador_fd, INSTANCE) != 1) {
				log_error(logger, "Fallo enviando el handshake");
				close(coordinador_fd);
				exit_gracefully(EXIT_FAILURE);
			}

			bool confirmation;
		int received = receive_confirmation(coordinador_fd, &confirmation);
			if (!received || !confirmation) {
				log_error(logger, "Fallo en la recepcion de la confirmacion");
				close(coordinador_fd);
				exit_gracefully(EXIT_FAILURE);
			}

		log_info(logger, "Conectado al Coordinador");

	//

	for(;;);

	exit_gracefully(EXIT_SUCCESS);
}


void exit_gracefully(int status) {
	log_info(logger, "Finalizo la ejecucion de la instancia");

	log_destroy(logger);

	exit(status);
}

#include "../libs/conector.h"
#include "esi.h"
#include "config.h"

/* -- Global variables -- */

t_esi_config setup;

t_log* logger;

int main(void) {

	logger = init_log();

	setup = init_config(logger);

	char* ip_coordinador = setup.ip_coordinador;
	char* ip_planificador = setup.ip_planificador;
	int port_coordinator = setup.port_coordinador;
	int port_scheduler = setup.port_planificador;

	bool confirmation;

	int coordinator_fd = connect_to_server(ip_coordinador, port_coordinator);
		if (send_handshake(coordinator_fd, ESI) != 1) {
			log_error(logger, "Fallo en el envío del handshake al coordinador");
			close(coordinator_fd);
			exit_gracefully(EXIT_FAILURE);
		}

	int received = receive_confirmation(coordinator_fd, &confirmation);
		if (!received || !confirmation) {
			log_error(logger, "Fallo en la confirmación de recepción de parte del coordinador");
			close(coordinator_fd);
			exit_gracefully(EXIT_FAILURE);
		}

	log_info(logger, "Conectado satisfactoriamente al coordinador");

	int scheduler_fd = connect_to_server(ip_planificador, port_scheduler);
		if (send_handshake(scheduler_fd, ESI) != 1) {
			log_error(logger, "Fallo en el envío del handshake al planificador");
			close(scheduler_fd);
		}


	received = receive_confirmation(scheduler_fd, &confirmation);
		if (!received || !confirmation) {
			log_error(logger, "Fallo en la confirmación de recepción de parte del planificador");
			close(scheduler_fd);
		}

    log_info(logger, "Conectado satisfactoriamente al planificador");

	while(1) {


	}

	exit_gracefully(EXIT_SUCCESS);
}

void exit_gracefully(int status) {

	log_info(logger, "La ejecución del ESI finalizó");

	log_destroy(logger);

	exit(status);
}

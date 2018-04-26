#include "../libs/conector.h"
#include "esi.h"
#include "config.h"
#include "../libs/serializador.h"

/* -- Global variables -- */

t_esi_config setup;

FILE* fp;
t_log* logger;

int main(int argc, char **argv) {

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

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(argv[1], "r");

    if (fp == NULL){

        log_error(logger, "Fallo al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {

    	package_t* package = obtain_package_from_line(line);

    }

    if (line) free(line);

	exit_gracefully(EXIT_SUCCESS);
}

package_t obtain_package_from_line(char* line) {

	package_t* package;
    t_esi_operacion parsed = parse(line);

    if(parsed.valido){

        switch(parsed.keyword){

            case GET:
            	log_info(logger, "GET\tclave: <%s>", parsed.argumentos.GET.clave);
            	/* TODO -- crear paquete para el caso de GET y los demás -- */
                break;
            case SET:
            	log_info(logger, "SET\tclave: <%s>\tvalor: <%s>", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
                break;
            case STORE:
            	log_info(logger, "STORE\tclave: <%s>", parsed.argumentos.STORE.clave);
                break;
            default:
            	log_error(logger, "No pude interpretar <%s>", line);
            	free(line);
                exit(EXIT_FAILURE);
        }

    } else {
    	log_error(logger, "La linea <%s> no es valida", line);
    	free(line);
    	exit_gracefully(EXIT_FAILURE);
    }

    destruir_operacion(parsed);

    return package;

}

void exit_gracefully(int status) {

	log_info(logger, "La ejecución del ESI finalizó");

	log_destroy(logger);

    fclose(fp);

	exit(status);
}

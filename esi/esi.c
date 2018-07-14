#include "../libs/conector.h"
#include "esi.h"
#include "config.h"
#include "../libs/serializador.h"
#include "../protocolo/protocolo.h"

/* -- Global variables -- */

t_esi_config setup;

FILE* fp;
t_log* logger;

int coordinator_fd_;
int scheduler_fd_;

int main(int argc, char **argv) {

	logger = init_log();

	setup = init_config(logger);

	char* ip_coordinador = setup.ip_coordinador;
	char* ip_planificador = setup.ip_planificador;
	int port_coordinator = setup.port_coordinador;
	int port_scheduler = setup.port_planificador;

	bool confirmation;

	coordinator_fd_ = connect_to_server(ip_coordinador, port_coordinator);
		if (send_handshake(coordinator_fd_, ESI) != 1) {
			log_error(logger, "Fallo en el envío del handshake al coordinador");
			close(coordinator_fd_);
			exit_gracefully(EXIT_FAILURE);
		}

	int received = receive_confirmation(coordinator_fd_, &confirmation);
		if (!received || !confirmation) {
			log_error(logger, "Fallo en la confirmación de recepción de parte del coordinador");
			close(coordinator_fd_);
			exit_gracefully(EXIT_FAILURE);
		}

	log_info(logger, "Conectado satisfactoriamente al coordinador");

	scheduler_fd_ = connect_to_server(ip_planificador, port_scheduler);
		if (send_handshake(scheduler_fd_, ESI) != 1) {
			log_error(logger, "Fallo en el envío del handshake al planificador");
			close(scheduler_fd_);
		}

	received = receive_confirmation(scheduler_fd_, &confirmation);
		if (!received || !confirmation) {
			log_error(logger, "Fallo en la confirmación de recepción de parte del planificador");
			close(scheduler_fd_);
		}

    log_info(logger, "Conectado satisfactoriamente al planificador");

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    char* route = string_from_format("scripts/%s", argv[argc - 1]);

    fp = fopen(route, "r");

    free(route);

    if (fp == NULL){

        log_error(logger, "Fallo al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    size_t package_size;
    int continue_reading_flag;
    read = getline(&line, &len, fp);

    while (read != -1) {

    	continue_reading_flag = 1;

    	wait_for_execution_order(scheduler_fd_);

    	void* package = obtain_package_from_line(line, &package_size);

    	send_serialized_package(coordinator_fd_, package, package_size);

    	free(package);

    	protocol_id execution_result = wait_for_execution_result(coordinator_fd_);
    	protocol_id execution_result_to_scheduler;

    	switch(execution_result) {

    	case PROTOCOL_CE_EVERYTHING_OK:
    		execution_result_to_scheduler = PROTOCOL_EP_EXECUTION_SUCCESS;
    		break;

    	case PROTOCOL_CE_YOU_ARE_BLOCKED:
    		execution_result_to_scheduler = PROTOCOL_EP_I_AM_BLOCKED;
    		continue_reading_flag = 0;
    		break;

    	case PROTOCOL_CE_ILLEGAL_OPERATION:
    		execution_result_to_scheduler = PROTOCOL_EP_I_BROKE_THE_LAW;
			log_error(logger, "Se ha producido una falla inesperada");
			free(line);
			exit_gracefully(EXIT_FAILURE);
    		break;

    	}

    	if (send(scheduler_fd_, &execution_result_to_scheduler, sizeof(execution_result_to_scheduler), 0) != sizeof(execution_result_to_scheduler)) {

    		log_error(logger, "Error al enviar el resultado de ejecución al planificador");
    		free(line);
    		exit_gracefully(EXIT_FAILURE);
    	}

    	if(continue_reading_flag) {

    		read = getline(&line, &len, fp);

    		if(execution_result_to_scheduler == PROTOCOL_EP_EXECUTION_SUCCESS) {
        	protocol_id script_end;

    		if(read == -1) {

    			script_end = PROTOCOL_EP_FINISHED_SCRIPT;
    		} else {

    			script_end = PROTOCOL_EP_DIDNT_FINISH_SCRIPT;
    		}

    		send(scheduler_fd_, &script_end, sizeof(script_end), 0);

    		}
    	}

    }

    if (line) free(line);

	exit_gracefully(EXIT_SUCCESS);
}

void* obtain_package_from_line(char* line, size_t* package_size) {

	package_t* package;
    t_esi_operacion parsed = parse(line);

    if(parsed.valido){

    	protocol_id operation;

        switch(parsed.keyword){

            case GET:

            	log_info(logger, "\x1b[97mGET\x1b[97m\tclave: <%s>", parsed.argumentos.GET.clave);
            	operation = PROTOCOL_EC_GET;
            	*package_size = sizeof(operation) + sizeof(size_t) + strlen(parsed.argumentos.GET.clave) + 1;
            	package = create_package(*package_size);
            	add_content(package, &operation, sizeof(operation));
            	add_content_variable(package, parsed.argumentos.GET.clave, strlen(parsed.argumentos.GET.clave) + 1);
                break;
            case SET:

            	log_info(logger, "\x1b[97mSET\x1b[97m\tclave: <%s>\tvalor: <%s>", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
            	operation = PROTOCOL_EC_SET;
            	*package_size = sizeof(operation) + sizeof(size_t) +strlen(parsed.argumentos.SET.clave) + 1 + sizeof(size_t) + strlen(parsed.argumentos.SET.valor) + 1;
            	package = create_package(*package_size);
            	add_content(package, &operation, sizeof(operation));
            	add_content_variable(package, parsed.argumentos.SET.clave, strlen(parsed.argumentos.SET.clave) + 1);
            	add_content_variable(package, parsed.argumentos.SET.valor, strlen(parsed.argumentos.SET.valor) + 1);
                break;
            case STORE:

            	log_info(logger, "\x1b[97mSTORE\x1b[97m\tclave: <%s>", parsed.argumentos.STORE.clave);
            	operation = PROTOCOL_EC_STORE;
            	*package_size = sizeof(operation) + sizeof(size_t) + strlen(parsed.argumentos.STORE.clave) + 1;
            	package = create_package(*package_size);
            	add_content(package, &operation, sizeof(operation));
            	add_content_variable(package, parsed.argumentos.STORE.clave, strlen(parsed.argumentos.STORE.clave) + 1);
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

    void *load = build_package(package);
    destroy_package(package);

    return load;
}

void wait_for_execution_order(int scheduler_fd) {

    protocol_id execution_order;
	if(recv(scheduler_fd, &execution_order, sizeof(execution_order), MSG_WAITALL) != sizeof(execution_order)) {

		log_error(logger, "Fallo en el receive de la orden de ejecución");
		exit_gracefully(EXIT_FAILURE);
	}

	if(execution_order != PROTOCOL_PE_EXEC) {

		log_error(logger, "La orden de ejecución no se recibió correctamente");
		exit_gracefully(EXIT_FAILURE);
	}
}

protocol_id wait_for_execution_result(int coordinador_fd_) {

	protocol_id execution_result;
	if (recv(coordinator_fd_, &execution_result, sizeof(execution_result), MSG_WAITALL) != sizeof(execution_result)) {

		log_error(logger, "Fallo en el receive del resultado de ejecución");
		exit_gracefully(EXIT_FAILURE);
	}

	return execution_result;
}

void exit_gracefully(int status) {

	if(status == EXIT_SUCCESS) {

	log_info(logger, "La ejecución del ESI finalizó");
	} else {

		log_error(logger, "La ejecución del ESI falló inesperadamente");
	}
	log_destroy(logger);

	close(coordinator_fd_);

	close(scheduler_fd_);

    fclose(fp);

	exit(status);
}

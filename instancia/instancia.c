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

t_list* data_saving_administrative_structure;

t_list* data_saving_structure;

int coordinador_fd;




// Main thread

int main(void) {

	logger = init_log();

	setup = init_config(logger);

	//Inicializo variables de creacion


	coordinador_fd = connect_to_server(setup.coordinator_ip, setup.coordinator_port);

	log_info(logger, "Conectando al coordinador...");

	handshake_client(coordinador_fd,setup.instance_name,INSTANCE,logger);

	data_saving_administrative_structure = list_create();

	data_saving_structure = list_create();


	//

	for(;;);

	exit_gracefully(EXIT_SUCCESS);
}


void exit_gracefully(int status) {
	log_info(logger, "Finalizo la ejecucion de la instancia");

	log_destroy(logger);

	exit(status);
}

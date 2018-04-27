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

int max_data_saving_structure_entries;

int size_data_saving_structure_entries;

int coordinador_fd;




// Main thread

int main(void) {

	logger = init_log();

	setup = init_config(logger);


	t_list* data_saving_administrative_structure;

	t_list* data_saving_structure;

	data_saving_administrative_structure_t data_administrative;

	data_saving_structure_t data;


	coordinador_fd = connect_to_server(setup.coordinator_ip, setup.coordinator_port);

	log_info(logger, "Conectando al coordinador...");

	handshake_client(coordinador_fd,setup.instance_name,INSTANCE,logger);




	//Coordinador me devuelve la cantidad de entradas disponibles

	//Coordinador me devuelve el tamano de cada entrada

	data_saving_administrative_structure = initialize_data_saving_administrative_structure()

	//

	for(;;);

	exit_gracefully(EXIT_SUCCESS);
}


void exit_gracefully(int status) {
	log_info(logger, "Finalizo la ejecucion de la instancia");

	log_destroy(logger);

	exit(status);
}

bool check_data_saving_structure_size()
{
	if (actual_data_saving_structure_entries_occupied<11)
		return FREE_SPACE_LEFT;
	return NOT_FREE_SPACE_LEFT;
}

t_list* initialize_data_saving_administrative_structure(data_saving_administrative_structure_t data)
{

t_list * data_saving_administrative_structure;

data_saving_administrative_structure = list_create();

for (int i=0;i<max_data_saving_structure_entries;i++)
{
	data.key = malloc(sizeof(char)*KEY_SIZE);

	data.entrie_size = 0;

	data.entrie_number = i;

	list_add(data_saving_administrative_structure,data);
}

return data_saving_administrative_structure;

}

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

	//Almacena en tiempo real la cantidad de entradas totales de las estructuras de almacenaje de informacion
	int actual_data_saving_structure_entries_occupied;

	data_saving_administrative_structure_t data_saving_administrative_structure_config;

	data_saving_structure_t data_saving_structure_config;

	t_list* data_saving_administrative_structure;

	t_list* data_saving_structure;




	coordinador_fd = connect_to_server(setup.coordinator_ip, setup.coordinator_port);

	log_info(logger, "Conectando al coordinador...");

	handshake_client(coordinador_fd,setup.instance_name,INSTANCE,logger);




	//Coordinador me devuelve la cantidad de entradas disponibles

	//Coordinador me devuelve el tamano de cada entrada

	data_saving_administrative_structure = initialize_data_saving_administrative_structure();

	//

	for(;;);

	exit_gracefully(EXIT_SUCCESS);
}


void exit_gracefully(int status) {
	log_info(logger, "Finalizo la ejecucion de la instancia");

	log_destroy(logger);

	exit(status);
}

bool free_space_left_data_saving_structure (int actual_data_saving_structure_entries_occupied)
{
	if (actual_data_saving_structure_entries_occupied<11)
		return FREE_SPACE_LEFT;
	return NOT_FREE_SPACE_LEFT;
}

t_list* initialize_data_saving_administrative_structure()
{

t_list * data_saving_administrative_structure;

data_saving_administrative_structure_t node;

data_saving_administrative_structure = list_create();

for (int i=0;i<max_data_saving_structure_entries;i++)
{
	node.key = malloc(sizeof(char)*KEY_SIZE);

	node.entrie_size = 0;

	node.entrie_number = i;

	list_add(data_saving_administrative_structure,node);
}

return data_saving_administrative_structure;

}

t_list* initialize_data_saving_structure()
{

t_list * data_saving_structure;

data_saving_administrative_structure_t node;

data_saving_structure = list_create();

for (int i=0;i<max_data_saving_structure_entries;i++)
{
	node.key = malloc(sizeof(char)*KEY_SIZE);

	node.entrie_size = 0;

	node.entrie_number = i;

	list_add(data_saving_structure,node);
}

return data_saving_structure;

}

bool try_to_save_SET_instruction_V1 (t_list * data_saving_administrative_structure,t_list * data_saving_structure,int actual_data_saving_structure_entries_occupied)
{
	char* set_instrction; //Esto lo voy a recibir del coordinador en uno de los parametros de las funcion
	char* key;
	//Tengo que checkear que no exista todavia

	if (free_space_left_data_saving_structure(actual_data_saving_structure_entries_occupied))
	{
		data_saving_administrative_structure_t node_administrative_struture;


		int total_entries_needed=total_entries_needed(set_instrction);
		//Debo checkar de la estructura de datos donde esta el primer nodo libre el cual entre la instruccion.
		//Si ocupa mas lugar del disponible en un nodo, busco donde hay x nodos consecutivos que entre el guardado de datos.


		data_saving_structure_t node_saving_struture;
	}

}

//Me deuvle la posicion del primer nodo donde puedo guardar la cantidad de entradas totales
int where_can_i_save_data(t_list * data_saving_structure,int total_entries_needed)
{

}

int total_entries_needed(char* SET_INST)
{
	int total_entrie_size = strlen(SET_INST);
	int total_entries_needed=0;
	while (total_entrie_size>0);
	{
		total_entries_needed++;
		total_entrie_size -= size_data_saving_structure_entries;
	}

	return total_entries_needed;
}

//TODO: Fn para checkear cuales son los primeros nodos en memoria libres donde puedo guardar info.....

/*-----------------------------------------ACLARACIONES GENERALES-------------------------------------------------- */

//ACLARACION: Los nodos en la estructura administrativa se borran, los nodos en memoria se vacian dejandolos disponibles pero sin datos.

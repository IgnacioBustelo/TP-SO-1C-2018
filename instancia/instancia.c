#include "../libs/configurator.h"
#include "../libs/messenger.h"
#include "algorithms.h"
#include "cfg_instancia.h"
#include "coordinator_api.h"
#include "dumper.h"
#include "entry_table.h"
#include "event_handler.h"
#include "instancia.h"
#include "storage.h"

#define IP					cfg_instancia_get_coordinador_ip()
#define HOST				cfg_instancia_get_coordinador_port()
#define NAME				cfg_instancia_get_instance_name()
#define MOUNT_POINT			cfg_instancia_get_mount_point()

// TODO: Agregar messenger_show() por aca y sacarlos de las funciones principales

void instance_init(char* process_name, char* logger_route, char* log_level, char* cfg_route) {
	storage_setup_t dimensions;

	event_handler_init();

	messenger_init(logger_route, process_name, log_level);

	cfg_instancia_init(cfg_route);

	coordinator_api_connect(IP, HOST);

	coordinator_api_handshake(NAME, &dimensions);

	storage_setup_init(dimensions.total_entries, dimensions.entry_size);

	storage_init(dimensions.total_entries, dimensions.entry_size);

	dumper_init(MOUNT_POINT);

	entry_table_init();

	algorithm_circular_set_pointer(4);

	messenger_show("INFO", "La Instancia se inicio correctamente");
}

int instance_set(key_value_t* key_value, t_list* replaced_keys) {
	messenger_show("INFO", "Se recibio un pedido de SET de la clave %s", key_value->key);

	int status = 1; // TODO: Hacer que cada operacion devuelva un estado y decidir si retornar o no error.


	if(!entry_table_have_entries(key_value)) {

		algorithm_circular(entry_table,key_value, replaced_keys);

		entry_table_delete_few(replaced_keys);

		dumper_remove_key_value(key_value->key);
	}

	// TODO: Para mas adelante => Chequear si requiere compactar

	int next_entry = entry_table_next_entry(key_value);

	storage_set(next_entry, key_value->value, key_value->size);

	status = entry_table_insert(next_entry, key_value);

	if(status == 1) {
		messenger_show("INFO", "Se proceso correctamente el SET de la clave %s en la entrada %d", key_value->key, next_entry);
	}

	return status;
}

int	instance_store(char* key) {
	messenger_show("INFO", "Se recibio un pedido de STORE de la clave %s", key);

	int status = 1; // TODO: Hacer que cada operacion devuelva un estado y decidir si retornar o no error.

	entry_t* entry = entry_table_get_entry(key); // TODO: Hacer que dada una clave, devuelva la entrada y el tamaño de la clave

	void* data = storage_retrieve(entry->number, entry->size);

	dumper_store(key, data, entry->size);

	if(status == 1) {
		messenger_show("INFO", "Se proceso correctamente el STORE de la clave %s", key);
	}

	free(data);

	return status;
}

void instance_main() {
	messenger_show("INFO", "Comienzo de actividades de la Instancia");

	while(instance_is_alive) {

		messenger_show("INFO", "Esperando peticion del Coordinador");

		int status; // TODO: Manejar el tema de retornos de valores de operaciones.

		switch(coordinator_api_receive_header()) {
			case PROTOCOL_CI_SET: {
				t_list* replaced_keys = list_create();

				key_value_t* key_value = coordinator_api_receive_set();

				status = instance_set(key_value, replaced_keys);

				key_value_destroy(key_value);

				if(!list_is_empty(replaced_keys)) {

					list_clean_and_destroy_elements(replaced_keys, free);
				}

				list_destroy(replaced_keys);

				// TODO: Añadir parametro de lista de claves reemplazadas
				coordinator_api_notify_set(status, get_total_entries() - entries_left);

				break;
			}

			case PROTOCOL_CI_STORE: {
				char* key = coordinator_api_receive_store();

				status = instance_store(key);

				coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, status);

				break;
			}

			case PROTOCOL_CI_KILL: {
				instance_is_alive = false;

				messenger_show("INFO", "La Instancia recibio un pedido del Coordinador para desconectarse");

				break;
			}

			default: {
				instance_is_alive = false;

				messenger_show("INFO", "Se recibio un mensaje no esperado");

				break;
			}
		}

	}
}

void instance_show() {
	entry_table_print_table();

	storage_show();

	// dumper_show();
}

void instance_die() {
	messenger_show("INFO", "Fin de actividades de la Instancia");

	storage_destroy();

	dumper_destroy();

	// entry_table_destroy(); TODO: Falta el destroyer de la tabla de entradas

	configurator_destroy();

	messenger_destroy();
}

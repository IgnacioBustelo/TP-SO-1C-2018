#include "../libs/configurator.h"
#include "../libs/messenger.h"
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

	// dumper_init(MOUNT_POINT);

	entry_table_init();
}

void instance_set(key_value_t* key_value) {
	// HAVE ENTRIES????????? (Es HASSSSSSSSSSSSSSSS!!!!!!!!!)

	if(!entry_table_have_entries(key_value)) {
		// TODO: Agregar logica de reemplazo y parametros necesarios a la funcion

		cfg_instancia_invoke_replacement_algorithm();
	}

	int next_entry = entry_table_next_entry(key_value);

	entry_table_insert(next_entry, key_value);

	storage_set(next_entry, key_value->value, key_value->size);
}

int	instance_store(char* key) {
	int status = 1; // TODO: Hacer que cada operacion devuelva un estado.

	entry_t* entry = entry_table_get_entry(key); // TODO: Hacer que dada una clave, devuelva la entrada y el tamaño de la clave

	void* data = storage_retrieve(entry->number, entry->size);

	dumper_store(key, data, entry->size);

	free(entry);

	return status;
}

void instance_main() {
	while(instance_is_alive) {

		int status; // TODO: Manejar el tema de retornos de valores de operaciones.

		switch(coordinator_api_receive_header()) {
			case PROTOCOL_CI_SET: {
				key_value_t* key_value = coordinator_api_receive_set();

				instance_set(key_value);

				key_value_destroy(key_value);

				status = 1; // TODO: Status tiene que ser lo que devuelva instance_set();

				coordinator_api_notify_set(status, get_total_entries() - entries_left);

				break;
			}

			case PROTOCOL_CI_STORE: {
				char* key = coordinator_api_receive_store();

				messenger_show("INFO", "Se recibio un pedido de STORE de la clave %s", key);

				status = instance_store(key);

				coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, status);

				break;
			}

			case PROTOCOL_CI_KILL: {
				instance_is_alive = false;

				break;
			}

			default: {
				messenger_show("INFO", "Se recibio un mensaje no esperado");

				break;
			}
		}

	}
}

void instance_show() {
	entry_table_print_table();

	storage_show();
}

void instance_die() {
	messenger_show("INFO", "Desconectando a la Instancia");

	storage_destroy();

	// entry_table_destroy(); TODO: Falta el destroyer de la tabla de entradas

	configurator_destroy();

	messenger_destroy();
}

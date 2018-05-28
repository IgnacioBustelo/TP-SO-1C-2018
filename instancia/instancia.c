#include "../libs/configurator.h"
#include "../libs/messenger.h"
#include "coordinator_api.h"
#include "entry_table.h"
#include "instancia.h"
#include "storage.h"
#include "terminator.h"

void instance_set(key_value_t* key_value) {
	int next_entry = entry_table_next_entry(key_value);

	entry_table_insert(next_entry, key_value);

	storage_set(next_entry, key_value->value, key_value->size);
}

void instance_die() {
	messenger_show("INFO", "Desconectando a la Instancia");

	storage_destroy();

	// entry_table_destroy(); TODO: Falta el destroyer de la tabla de entradas

	configurator_destroy();

	messenger_destroy();
}

void instance_main() {
	while(terminator_is_alive()) {

		int status; // TODO: Manejar el tema de retornos de valores de operaciones.

		switch(coordinator_api_receive_header()) {
			case PROTOCOL_CI_SET: {
				key_value_t* key_value = coordinator_api_receive_set();

				instance_set(key_value);

				key_value_destroy(key_value);

				status = 1;

				break;
			}

			case PROTOCOL_CI_KILL: {
				terminator_die();

				status = 0;

				break;
			}

			default: {
				status = 0;

				break;
			}
		}

		coordinator_api_notify_set(status, get_total_entries() - entries_left);

	}
}

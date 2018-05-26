#include "../libs/logger.h"
#include "instancia.h"
#include "cfg_instancia.h"
#include "coordinator_api.h"
#include "globals.h"

#define PROCESS			"Instancia"
#define LOGGER			"../instancia.log"
#define CFG				"../instancia.cfg"
#define IP				cfg_instancia_get_coordinador_ip()
#define HOST			cfg_instancia_get_coordinador_port()
#define NAME			cfg_instancia_get_instance_name()
#define ENTRIES_USED	instance_entries_used()

void main_mock() {
	cfg_instancia_init(CFG);

	configurator_read();

	storage_setup_init(0, 0);

	coordinator_api_handshake(NAME, storage_setup);

	entry_table_init();

	storage_init(get_total_entries(), get_entry_size());

	for(;;) {

		int status;

		switch(coordinator_api_receive_header()) {
			case PROTOCOL_CI_SET: {
				messenger_show("INFO", "Se recibio un pedido de SET");

				key_value_t* key_value = coordinator_api_receive_set();

				instance_set(key_value);

				key_value_destroy(key_value);

				entry_table_print_table();

				storage_show();

				status = 1;

				break;
			}

			default: {
				status = 0;

				break;
			}
		}

		messenger_show("INFO", "Resultado de la operacion: Status = %d; Entradas usadas = %d", status, ENTRIES_USED);

		coordinator_api_notify_set(status, ENTRIES_USED);

	}
}

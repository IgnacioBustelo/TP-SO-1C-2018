#include "../libs/messenger.h"
#include "cfg_instancia.h"
#include "coordinator_api.h"
#include "entry_table.h"
#include "globals.h"
#include "storage.h"
#include "terminator.h"

#define IP				cfg_instancia_get_coordinador_ip()
#define HOST			cfg_instancia_get_coordinador_port()
#define NAME			cfg_instancia_get_instance_name()
#define MOUNT_POINT		cfg_instancia_get_mount_point()

void bootstrap_start(char* process_name, char* logger_route, char* log_level, char* cfg_route) {
	storage_setup_t dimensions;

	terminator_init();

	messenger_init(logger_route, process_name, log_level);

	cfg_instancia_init(cfg_route);

	coordinator_api_connect(IP, HOST);

	coordinator_api_handshake(NAME, &dimensions);

	storage_setup_init(dimensions.total_entries, dimensions.entry_size);

	storage_init(MOUNT_POINT, dimensions.total_entries, dimensions.entry_size);

	entry_table_init();
}

void bootstrap_show() {
	cfg_instancia_show();

	entry_table_print_table();

	storage_show();
}

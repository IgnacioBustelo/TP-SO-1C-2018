#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>

#include "../../libs/mocks/client_server.h"
#include "../../libs/mocks/printfer.h"

#include "../coordinator_api.h"
#include "../instancia.h"
#include "coordinador_mock.h"

sem_t	test_sem, handshake_sem;

bool	is_testing;
int		total_entries, entry_size, fd_instancia;
char*	current_cfg;

static void handshake(t_list* recoverable_keys) {
	bool is_accepted = true;

	char* received_name;

	coordinador_mock_handshake_base(fd_instancia, &is_accepted);

	coordinador_mock_handshake_receive_name(fd_instancia, &received_name);

	coordinador_mock_handshake_send_config(fd_instancia, received_name, total_entries, entry_size, recoverable_keys);

	free(received_name);

	sem_wait(&handshake_sem);
}

static void set(char* key, char* value, bool is_new) {
	coordinador_mock_set_request(fd_instancia, is_new, key, value);

	coordinador_mock_set_response(fd_instancia);
}

static void store(char* key) {
	coordinador_mock_store_request(fd_instancia, key);

	coordinador_mock_store_response(fd_instancia);
}

static void status(char* key) {
	coordinador_mock_status_request(fd_instancia, key);

	free(coordinador_mock_status_response(fd_instancia, key));
}

static void test_template_full(char* test_message, char* cfg, t_list* recoverable_keys, void(*test)(void)) {
	current_cfg = cfg;

	messenger_show("INFO", "%s%s%s", COLOR_MAGENTA, test_message, COLOR_RESET);

	sem_post(&test_sem);

	handshake(recoverable_keys);

	test();

	messenger_show("INFO", "%sFin de la prueba: '%s'%s", COLOR_MAGENTA, test_message, COLOR_RESET);

	list_destroy_and_destroy_elements(recoverable_keys, free);
}

static void test_template(char* test_message, char* cfg, void(*test)(void)) {
	t_list* empty_list = list_create();

	test_template_full(test_message, cfg, empty_list, test);
}

static void test_1() {
	set("A", "XXX", true);

	set("B", "YYYYY", true);

	set("C", "ZZZZ", true);

	store("A");

	store("C");

	status("A");

	status("B");
}

void client_server_execute_server(int fd_client) {
	fd_instancia = fd_client;

	test_template("Prueba simple de SET, STORE, y STATUS", "test_1", test_1);

	is_testing = false;

	coordinador_mock_kill(fd_instancia);
}

void client_server_execute_client(int fd_server) {
	is_closable_client = false;

	fd_coordinador = fd_server;

	while(is_testing) {
		char* cfg_path = string_from_format("final_tests_cfg/%s.cfg", current_cfg);

		instance_init("Instancia", "../instancia.log", "INFO", cfg_path);

		sem_post(&handshake_sem);

		instance_main();

		instance_die();

		free(cfg_path);
	}
}

int main() {
	sem_init(&test_sem, 0, 0);
	sem_init(&handshake_sem, 0, 0);

	server_name = "Coordinador";
	client_name = "Instancia 1";

	total_entries = 8;
	entry_size = 4;

	is_testing = true;

	client_server_run();

	sem_destroy(&test_sem);
	sem_destroy(&handshake_sem);
}

#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>

#include "../../libs/mocks/client_server.h"
#include "../../libs/mocks/printfer.h"

#include "../coordinator_api.h"
#include "../dumper.h"
#include "../entry_table.h"
#include "../instancia.h"
#include "../storage.h"
#include "coordinador_mock.h"

sem_t	test_sem, handshake_sem;

int		chosen_test, total_entries, entry_size, fd_instancia;
char*	current_cfg;

static void set(char* key, char* value) {
	coordinador_mock_set(fd_instancia, key, value);

	entry_table_show();

	storage_show();
}

static void store(char* key) {
	coordinador_mock_store(fd_instancia, key);

	dumper_show();
}

static void status(char* key) {
	coordinador_mock_status(fd_instancia, key);
}

static void test_template_full(char* test_message, char* cfg, t_list* recoverable_keys, void(*test)(void)) {
	coordinador_mock_init();

	current_cfg = cfg;

	messenger_show("INFO", "%s%s%s", COLOR_MAGENTA, test_message, COLOR_RESET);

	sem_post(&test_sem);

	coordinador_mock_handshake(fd_instancia, total_entries, entry_size, recoverable_keys);

	sem_wait(&handshake_sem);

	test();

	coordinador_mock_kill(fd_instancia);

	messenger_show("INFO", "%sFin de la prueba: '%s'%s", COLOR_MAGENTA, test_message, COLOR_RESET);

	list_destroy_and_destroy_elements(recoverable_keys, free);

	coordinador_mock_destroy();
}

static void test_template(char* test_message, void(*test)(void)) {
	t_list* empty_list = list_create();

	test_template_full(test_message, "test", empty_list, test);
}

static void test_1() {
	set("A", "WWW");

	set("B", "XXXXX");

	set("C", "YYYY");

	set("D", "ZZZZZZZZZZZZZZZZ");

	store("A");

	store("C");

	status("A");

	status("D");
}

static void test_2() {
	set("A", "AAAAAAAAAA");

	set("B", "BBBBBB");

	set("A", "CCCCCCCCCC");

	set("A", "DDD");

	set("A", "EEEEEE");

	set("C", "F");

	set("A", "EEEEEE");

	set("A", "GGGGGGGG");

	set("A", "HHHHHHHHH");
}

static void test_3() {
	set("nintendo:consola:nes", "Mario");

	set("nintendo:consola:snes", "SuperMario");

	set("nintendo:consola:nintendo64", "Mario64");

	set("nintendo:consola:gamecube", "FinalFantasyTactics");

	set("nintendo:consola:wii", "MarioGalaxy");

	set("nintendo:consola:wii", "MarioGalaxy2");

	set("nintendo:consola:wiiu", "RIP");

	set("nintendo:consola:nintendo64", "MarioKart");

	set("sony:consola:ps1", "FF7");

	set("nintendo:consola:nintendo64", "PkmnSnap");

	set("sony:consola:ps2", "GodOfWar");

	set("sony:consola:ps1", "FF8");

	set("sony:consola:ps3", "TLOU");
}

void client_server_execute_server(int fd_client) {
	fd_instancia = fd_client;

	switch(chosen_test) {
		case 1: test_template("Test 1: Prueba simple de SET, STORE, y STATUS", test_1);			break;

		case 2: test_template("Test 2: Prueba de multiples SET sobre la misma clave", test_2);	break;

		case 3: test_template("Test 3: Prueba de reemplazo de la catedra", test_3);				break;

		default: messenger_show("ERROR", "Test desconocido");									break;
	}
}

void client_server_execute_client(int fd_server) {
	is_closable_client = false;

	fd_coordinador = fd_server;

	char* cfg_path = string_from_format("final_tests_cfg/%s.cfg", current_cfg);

	instance_init("Instancia", "../instancia.log", "INFO", cfg_path);

	sem_post(&handshake_sem);

	instance_main();

	instance_die();

	free(cfg_path);
}

int main(int argc, char* argv[]) {
	sem_init(&test_sem, 0, 0);
	sem_init(&handshake_sem, 0, 0);

	server_name = "Coordinador";
	client_name = "Instancia 1";

	chosen_test = (argc > 1) ? atoi(argv[1]) : 1;
	total_entries = (argc > 2) ? atoi(argv[2]) : 8;
	entry_size = (argc > 3) ? atoi(argv[3]) : 4;

	client_server_run();

	sem_destroy(&test_sem);
	sem_destroy(&handshake_sem);
}

#include <stdio.h>
#include <stdlib.h>
#include "../consola.h"

static esi_information* create_esi_information_test(int esi_id, int esi_numeric_name, double estimation);

static void first_test();

static void second_test();

static void third_test();

static void forth_test();

static void fifth_test();

int main() {

	first_test();

	second_test();

	third_test();

	forth_test();

	fifth_test();
}

static void first_test() {

	printf("Primer test: ESI 1 y 2 deberían estar en deadlock\n");

	t_list* locked_keys = list_create();
	t_list* esi_requests = list_create();
	t_list* esis_in_system = list_create();

	list_add(esis_in_system, (void*) create_esi_information_test(1, 1, 1));
	list_add(esis_in_system, (void*) create_esi_information_test(2, 2, 2));

	list_add(locked_keys, (void*) create_key_blocker("R2", 1));
	list_add(locked_keys, (void*) create_key_blocker("R1", 2));

	list_add(esi_requests, (void*) create_esi_sexpecting_key(1, "R1"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(2, "R2"));

	detect_and_show_all_deadlocks(locked_keys, esi_requests, esis_in_system);
}

static void second_test() {

	printf("Segundo test: ESI 5, 6 y 7 deberían estar en deadlock\n");

	t_list* locked_keys = list_create();
	t_list* esi_requests = list_create();
	t_list* esis_in_system = list_create();

	list_add(esis_in_system, (void*) create_esi_information_test(1, 1, 1));
	list_add(esis_in_system, (void*) create_esi_information_test(2, 2, 2));
	list_add(esis_in_system, (void*) create_esi_information_test(3, 3, 3));
	list_add(esis_in_system, (void*) create_esi_information_test(4, 4, 4));
	list_add(esis_in_system, (void*) create_esi_information_test(5, 5, 5));
	list_add(esis_in_system, (void*) create_esi_information_test(6, 6, 6));
	list_add(esis_in_system, (void*) create_esi_information_test(7, 7, 7));

	list_add(locked_keys, (void*) create_key_blocker("R2", 2));
	list_add(locked_keys, (void*) create_key_blocker("R6", 2));
	list_add(locked_keys, (void*) create_key_blocker("R1", 3));
	list_add(locked_keys, (void*) create_key_blocker("R5", 4));
	list_add(locked_keys, (void*) create_key_blocker("R4", 5));
	list_add(locked_keys, (void*) create_key_blocker("R3", 6));
	list_add(locked_keys, (void*) create_key_blocker("R7", 7));

	list_add(esi_requests, (void*) create_esi_sexpecting_key(1, "R2"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(2, "R1"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(5, "R3"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(6, "R7"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(7, "R4"));

	detect_and_show_all_deadlocks(locked_keys, esi_requests, esis_in_system);
}

static void third_test() {

	printf("Tercer test: ESI 2 y 3 deberían estar en deadlock y el ESI 5, 6 y 7 en otro\n");

	t_list* locked_keys = list_create();
	t_list* esi_requests = list_create();
	t_list* esis_in_system = list_create();

	list_add(esis_in_system, (void*) create_esi_information_test(1, 1, 1));
	list_add(esis_in_system, (void*) create_esi_information_test(2, 2, 2));
	list_add(esis_in_system, (void*) create_esi_information_test(3, 3, 3));
	list_add(esis_in_system, (void*) create_esi_information_test(4, 4, 4));
	list_add(esis_in_system, (void*) create_esi_information_test(5, 5, 5));
	list_add(esis_in_system, (void*) create_esi_information_test(6, 6, 6));
	list_add(esis_in_system, (void*) create_esi_information_test(7, 7, 7));

	list_add(locked_keys, (void*) create_key_blocker("R2", 2));
	list_add(locked_keys, (void*) create_key_blocker("R6", 2));
	list_add(locked_keys, (void*) create_key_blocker("R1", 3));
	list_add(locked_keys, (void*) create_key_blocker("R5", 4));
	list_add(locked_keys, (void*) create_key_blocker("R4", 5));
	list_add(locked_keys, (void*) create_key_blocker("R3", 6));
	list_add(locked_keys, (void*) create_key_blocker("R7", 7));

	list_add(esi_requests, (void*) create_esi_sexpecting_key(1, "R2"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(2, "R1"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(5, "R3"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(6, "R7"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(7, "R4"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(3, "R2"));

	detect_and_show_all_deadlocks(locked_keys, esi_requests, esis_in_system);
}

static void forth_test() {

	printf("Cuarto test: No debería existir ningún deadlock\n");

	t_list* locked_keys = list_create();
	t_list* esi_requests = list_create();
	t_list* esis_in_system = list_create();

	list_add(esis_in_system, (void*) create_esi_information_test(1, 1, 1));
	list_add(esis_in_system, (void*) create_esi_information_test(2, 2, 2));
	list_add(esis_in_system, (void*) create_esi_information_test(3, 3, 3));
	list_add(esis_in_system, (void*) create_esi_information_test(4, 4, 4));
	list_add(esis_in_system, (void*) create_esi_information_test(5, 5, 5));
	list_add(esis_in_system, (void*) create_esi_information_test(6, 6, 6));

	list_add(locked_keys, (void*) create_key_blocker("R1", 2));
	list_add(locked_keys, (void*) create_key_blocker("R2", 3));
	list_add(locked_keys, (void*) create_key_blocker("R4", 5));
	list_add(locked_keys, (void*) create_key_blocker("R6", 6));

	list_add(esi_requests, (void*) create_esi_sexpecting_key(1, "R1"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(3, "R1"));
	list_add(esi_requests, (void*) create_esi_sexpecting_key(5, "R6"));

	detect_and_show_all_deadlocks(locked_keys, esi_requests, esis_in_system);
}

static void fifth_test() {

	printf("Quinto test: No debería existir ningún deadlock ya que sólo hay un ESI\n");

	t_list* locked_keys = list_create();
	t_list* esi_requests = list_create();
	t_list* esis_in_system = list_create();

	list_add(esis_in_system, (void*) create_esi_information_test(1, 1, 1));

	list_add(locked_keys, (void*) create_key_blocker("R2", 1));

	detect_and_show_all_deadlocks(locked_keys, esi_requests, esis_in_system);

}

static esi_information* create_esi_information_test(int esi_id, int esi_numeric_name, double estimation) {

	esi_information* esi_inf = malloc(sizeof(esi_information));
	esi_inf->esi_id = esi_id;
	esi_inf->esi_numeric_name = esi_numeric_name;
	esi_inf->last_estimated_burst = estimation;
	esi_inf->last_real_burst = 0;
	esi_inf->waited_bursts = 0;
	return esi_inf;
}


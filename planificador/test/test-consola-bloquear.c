#include <stdio.h>
#include <stdlib.h>
#include "../planificador.h"

static esi_information* create_esi_information_test(int esi_id, int esi_numeric_name, double estimation);

static esi_information* obtain_esi_information_by_id_test(int esi_fd, t_list* esi_bursts);

static void block_by_console_procedure_test(t_list* g_new_blocked_by_console_esis, t_list* g_esi_bursts, t_list* g_locked_keys, t_list* g_esis_sexpecting_keys, t_list* g_execution_queue, t_list* g_ready_queue, t_list* g_blocked_queue);

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

	printf("Primer test: Clave que nadie tiene\n");

	printf("Bloquear R4 2 -- ESI 2 hace GET de R4\n");

	t_list* esi_bursts = list_create();

	list_add(esi_bursts, (void*) create_esi_information_test(1, 2, 3));
	list_add(esi_bursts, (void*) create_esi_information_test(2, 3, 4));
	list_add(esi_bursts, (void*) create_esi_information_test(3, 4, 5));
	list_add(esi_bursts, (void*) create_esi_information_test(4, 5, 6));
	list_add(esi_bursts, (void*) create_esi_information_test(5, 6, 7));

	t_list* locked_keys = list_create();

	list_add(locked_keys, (void*) create_key_blocker("R1", 1));
	list_add(locked_keys, (void*) create_key_blocker("R2", 2));
	list_add(locked_keys, (void*) create_key_blocker("R3", 3));

	t_list* esis_sexpecting_key = list_create();

	list_add(esis_sexpecting_key, (void*) create_esi_sexpecting_key(4, "R1"));

	int* esi_id1 = malloc(sizeof(int));
	*esi_id1 = 1;
	int* esi_id2 = malloc(sizeof(int));
	*esi_id2 = 2;
	int* esi_id3 = malloc(sizeof(int));
	*esi_id3 = 3;
	int* esi_id4 = malloc(sizeof(int));
	*esi_id4 = 4;
	int* esi_id5 = malloc(sizeof(int));
	*esi_id5 = 5;

	t_list* execution_queue = list_create();

	list_add(execution_queue, (void*) esi_id1);

	t_list* ready_queue = list_create();

	list_add(ready_queue, (void*) esi_id2);
	list_add(ready_queue, (void*) esi_id3);
	list_add(ready_queue, (void*) esi_id5);

	t_list* blocked_queue = list_create();

	list_add(blocked_queue, (void*) esi_id4);

	t_list* new_blocked_by_console_esis = list_create();

	list_add(new_blocked_by_console_esis, (void*)create_esi_sexpecting_key(1, "R4"));

	block_by_console_procedure_test(new_blocked_by_console_esis, esi_bursts, locked_keys, esis_sexpecting_key, execution_queue, ready_queue, blocked_queue);
}

static void second_test() {

	printf("Segundo test: Puede bloquearse porque está en ready\n");

	printf("Bloquear R2 6 -- ESI 6 hace GET de R2\n");

	t_list* esi_bursts = list_create();

	list_add(esi_bursts, (void*) create_esi_information_test(1, 2, 3));
	list_add(esi_bursts, (void*) create_esi_information_test(2, 3, 4));
	list_add(esi_bursts, (void*) create_esi_information_test(3, 4, 5));
	list_add(esi_bursts, (void*) create_esi_information_test(4, 5, 6));
	list_add(esi_bursts, (void*) create_esi_information_test(5, 6, 7));

	t_list* locked_keys = list_create();

	list_add(locked_keys, (void*) create_key_blocker("R1", 1));
	list_add(locked_keys, (void*) create_key_blocker("R2", 2));
	list_add(locked_keys, (void*) create_key_blocker("R3", 3));

	t_list* esis_sexpecting_key = list_create();

	list_add(esis_sexpecting_key, (void*) create_esi_sexpecting_key(4, "R1"));

	int* esi_id1 = malloc(sizeof(int));
	*esi_id1 = 1;
	int* esi_id2 = malloc(sizeof(int));
	*esi_id2 = 2;
	int* esi_id3 = malloc(sizeof(int));
	*esi_id3 = 3;
	int* esi_id4 = malloc(sizeof(int));
	*esi_id4 = 4;
	int* esi_id5 = malloc(sizeof(int));
	*esi_id5 = 5;

	t_list* execution_queue = list_create();

	list_add(execution_queue, (void*) esi_id1);

	t_list* ready_queue = list_create();

	list_add(ready_queue, (void*) esi_id2);
	list_add(ready_queue, (void*) esi_id3);
	list_add(ready_queue, (void*) esi_id5);

	t_list* blocked_queue = list_create();

	list_add(blocked_queue, (void*) esi_id4);

	t_list* new_blocked_by_console_esis = list_create();

	list_add(new_blocked_by_console_esis, (void*)create_esi_sexpecting_key(5, "R2"));

	block_by_console_procedure_test(new_blocked_by_console_esis, esi_bursts, locked_keys, esis_sexpecting_key, execution_queue, ready_queue, blocked_queue);
}

static void third_test() {

	printf("Tercer test: Puede bloquearse porque está en ejecución\n");

	printf("Bloquear R2 2 -- ESI 2 hace GET de R2\n");

	t_list* esi_bursts = list_create();

	list_add(esi_bursts, (void*) create_esi_information_test(1, 2, 3));
	list_add(esi_bursts, (void*) create_esi_information_test(2, 3, 4));
	list_add(esi_bursts, (void*) create_esi_information_test(3, 4, 5));
	list_add(esi_bursts, (void*) create_esi_information_test(4, 5, 6));
	list_add(esi_bursts, (void*) create_esi_information_test(5, 6, 7));

	t_list* locked_keys = list_create();

	list_add(locked_keys, (void*) create_key_blocker("R1", 1));
	list_add(locked_keys, (void*) create_key_blocker("R2", 2));
	list_add(locked_keys, (void*) create_key_blocker("R3", 3));

	t_list* esis_sexpecting_key = list_create();

	list_add(esis_sexpecting_key, (void*) create_esi_sexpecting_key(4, "R1"));

	int* esi_id1 = malloc(sizeof(int));
	*esi_id1 = 1;
	int* esi_id2 = malloc(sizeof(int));
	*esi_id2 = 2;
	int* esi_id3 = malloc(sizeof(int));
	*esi_id3 = 3;
	int* esi_id4 = malloc(sizeof(int));
	*esi_id4 = 4;
	int* esi_id5 = malloc(sizeof(int));
	*esi_id5 = 5;

	t_list* execution_queue = list_create();

	list_add(execution_queue, (void*) esi_id1);

	t_list* ready_queue = list_create();

	list_add(ready_queue, (void*) esi_id2);
	list_add(ready_queue, (void*) esi_id3);
	list_add(ready_queue, (void*) esi_id5);

	t_list* blocked_queue = list_create();

	list_add(blocked_queue, (void*) esi_id4);

	t_list* new_blocked_by_console_esis = list_create();

	list_add(new_blocked_by_console_esis, (void*)create_esi_sexpecting_key(1, "R2"));

	block_by_console_procedure_test(new_blocked_by_console_esis, esi_bursts, locked_keys, esis_sexpecting_key, execution_queue, ready_queue, blocked_queue);
}

static void forth_test() {

	printf("Cuarto test: No puede bloquearse porque ya está bloqueado\n");

	printf("Bloquear R3 5 -- ESI 5 hace GET de R3\n");

	t_list* esi_bursts = list_create();

	list_add(esi_bursts, (void*) create_esi_information_test(1, 2, 3));
	list_add(esi_bursts, (void*) create_esi_information_test(2, 3, 4));
	list_add(esi_bursts, (void*) create_esi_information_test(3, 4, 5));
	list_add(esi_bursts, (void*) create_esi_information_test(4, 5, 6));
	list_add(esi_bursts, (void*) create_esi_information_test(5, 6, 7));

	t_list* locked_keys = list_create();

	list_add(locked_keys, (void*) create_key_blocker("R1", 1));
	list_add(locked_keys, (void*) create_key_blocker("R2", 2));
	list_add(locked_keys, (void*) create_key_blocker("R3", 3));

	t_list* esis_sexpecting_key = list_create();

	list_add(esis_sexpecting_key, (void*) create_esi_sexpecting_key(4, "R1"));

	int* esi_id1 = malloc(sizeof(int));
	*esi_id1 = 1;
	int* esi_id2 = malloc(sizeof(int));
	*esi_id2 = 2;
	int* esi_id3 = malloc(sizeof(int));
	*esi_id3 = 3;
	int* esi_id4 = malloc(sizeof(int));
	*esi_id4 = 4;
	int* esi_id5 = malloc(sizeof(int));
	*esi_id5 = 5;

	t_list* execution_queue = list_create();

	list_add(execution_queue, (void*) esi_id1);

	t_list* ready_queue = list_create();

	list_add(ready_queue, (void*) esi_id2);
	list_add(ready_queue, (void*) esi_id3);
	list_add(ready_queue, (void*) esi_id5);

	t_list* blocked_queue = list_create();

	list_add(blocked_queue, (void*) esi_id4);

	t_list* new_blocked_by_console_esis = list_create();

	list_add(new_blocked_by_console_esis, (void*)create_esi_sexpecting_key(4, "R3"));

	block_by_console_procedure_test(new_blocked_by_console_esis, esi_bursts, locked_keys, esis_sexpecting_key, execution_queue, ready_queue, blocked_queue);
}

static void fifth_test() {

	printf("Quinto test: No puede bloquearse porque el ESI no existe. Como no la tenía nadie la toma el sistema\n");

	printf("Bloquear R4 7 -- ESI 7 hace GET de R4\n");

	t_list* esi_bursts = list_create();

	list_add(esi_bursts, (void*) create_esi_information_test(1, 2, 3));
	list_add(esi_bursts, (void*) create_esi_information_test(2, 3, 4));
	list_add(esi_bursts, (void*) create_esi_information_test(3, 4, 5));
	list_add(esi_bursts, (void*) create_esi_information_test(4, 5, 6));
	list_add(esi_bursts, (void*) create_esi_information_test(5, 6, 7));

	t_list* locked_keys = list_create();

	list_add(locked_keys, (void*) create_key_blocker("R1", 1));
	list_add(locked_keys, (void*) create_key_blocker("R2", 2));
	list_add(locked_keys, (void*) create_key_blocker("R3", 3));

	t_list* esis_sexpecting_key = list_create();

	list_add(esis_sexpecting_key, (void*) create_esi_sexpecting_key(4, "R1"));

	int* esi_id1 = malloc(sizeof(int));
	*esi_id1 = 1;
	int* esi_id2 = malloc(sizeof(int));
	*esi_id2 = 2;
	int* esi_id3 = malloc(sizeof(int));
	*esi_id3 = 3;
	int* esi_id4 = malloc(sizeof(int));
	*esi_id4 = 4;
	int* esi_id5 = malloc(sizeof(int));
	*esi_id5 = 5;

	t_list* execution_queue = list_create();

	list_add(execution_queue, (void*) esi_id1);

	t_list* ready_queue = list_create();

	list_add(ready_queue, (void*) esi_id2);
	list_add(ready_queue, (void*) esi_id3);
	list_add(ready_queue, (void*) esi_id5);

	t_list* blocked_queue = list_create();

	list_add(blocked_queue, (void*) esi_id4);

	t_list* new_blocked_by_console_esis = list_create();

	list_add(new_blocked_by_console_esis, (void*)create_esi_sexpecting_key(6, "R4"));

	block_by_console_procedure_test(new_blocked_by_console_esis, esi_bursts, locked_keys, esis_sexpecting_key, execution_queue, ready_queue, blocked_queue);
}


static void block_by_console_procedure_test(t_list* g_new_blocked_by_console_esis, t_list* g_esi_bursts, t_list* g_locked_keys, t_list* g_esis_sexpecting_keys, t_list* g_execution_queue, t_list* g_ready_queue, t_list* g_blocked_queue) {

	bool is_executing(void* esi_key) {

		return *(int*)list_get(g_execution_queue, 0) == ((esi_sexpecting_key*)esi_key)->esi_fd;
	}

	if(!list_is_empty(g_execution_queue) && list_any_satisfy(g_new_blocked_by_console_esis, is_executing)) {

		esi_sexpecting_key* esi_sexpecting_key_exec = list_find(g_new_blocked_by_console_esis, is_executing);

		if(determine_if_key_is_blocked(esi_sexpecting_key_exec->key, g_locked_keys)) {

		list_add(g_esis_sexpecting_keys, (void*)create_esi_sexpecting_key(esi_sexpecting_key_exec->esi_fd, esi_sexpecting_key_exec->key));
		move_esi_from_and_to_queue(g_execution_queue, g_blocked_queue, esi_sexpecting_key_exec->esi_fd);
		printf("El ESI %i, que está en ejecución, se ha bloqueado por consola y ahora está esperando la liberación de la clave %s\n", obtain_esi_information_by_id_test(esi_sexpecting_key_exec->esi_fd, g_esi_bursts)->esi_numeric_name, esi_sexpecting_key_exec->key);
		} else {

			list_add(g_locked_keys, (void*)create_key_blocker(esi_sexpecting_key_exec->key, esi_sexpecting_key_exec->esi_fd));
			printf("El ESI %i, que está en ejecución, tomó la clave %s y no se bloqueó ya que nadie poseía la misma\n", obtain_esi_information_by_id_test(esi_sexpecting_key_exec->esi_fd, g_esi_bursts)->esi_numeric_name, esi_sexpecting_key_exec->key);
		}

		bool equality(void* esi_key) {

			return ((esi_sexpecting_key*)esi_key)->esi_fd == esi_sexpecting_key_exec->esi_fd;
		}

		void destroy_esi_sexpecting_key(void* esi_sexpecting_key_) {

			free(((esi_sexpecting_key*)esi_sexpecting_key_)->key);
			free(esi_sexpecting_key_);
		}

		list_remove_and_destroy_by_condition(g_new_blocked_by_console_esis, equality, destroy_esi_sexpecting_key);
	}

	bool is_in_ready(void* esi_key) {

		bool equality(void* esi_in_ready) {

			return ((esi_sexpecting_key*)esi_key)->esi_fd == *(int*)esi_in_ready;
		}

		return list_any_satisfy(g_ready_queue, equality);
	}

	if (!list_is_empty(g_ready_queue) && list_any_satisfy(g_new_blocked_by_console_esis, is_in_ready)) {

		t_list* esis_in_ready_to_be_blocked = list_filter(g_new_blocked_by_console_esis, is_in_ready);

		void closure(void* esi_key) {

			esi_sexpecting_key* esi_sexpecting_key_ready = (esi_sexpecting_key*)esi_key;

			if(determine_if_key_is_blocked(esi_sexpecting_key_ready->key, g_locked_keys)) {

			list_add(g_esis_sexpecting_keys, (void*)create_esi_sexpecting_key(esi_sexpecting_key_ready->esi_fd, esi_sexpecting_key_ready->key));
			move_esi_from_and_to_queue(g_ready_queue, g_blocked_queue, esi_sexpecting_key_ready->esi_fd);
			printf("El ESI %i, que está en ready, se ha bloqueado por consola y ahora está esperando la liberación de la clave %s\n", obtain_esi_information_by_id_test(esi_sexpecting_key_ready->esi_fd, g_esi_bursts)->esi_numeric_name, esi_sexpecting_key_ready->key);
			} else {

				list_add(g_locked_keys, (void*)create_key_blocker(esi_sexpecting_key_ready->key, esi_sexpecting_key_ready->esi_fd));
				printf("El ESI %i, que está en ready, tomó la clave %s y no se bloqueó ya que nadie poseía la misma\n", obtain_esi_information_by_id_test(esi_sexpecting_key_ready->esi_fd, g_esi_bursts)->esi_numeric_name, esi_sexpecting_key_ready->key);
			}

			bool equality(void* esi_key) {

				return ((esi_sexpecting_key*)esi_key)->esi_fd == esi_sexpecting_key_ready->esi_fd;
			}

			void destroy_esi_sexpecting_key(void* esi_sexpecting_key_) {

				free(((esi_sexpecting_key*)esi_sexpecting_key_)->key);
				free(esi_sexpecting_key_);
			}

			list_remove_and_destroy_by_condition(g_new_blocked_by_console_esis, equality, destroy_esi_sexpecting_key);
		}

		list_iterate(esis_in_ready_to_be_blocked, closure);
	}

	bool not_in_ready_nor_exec(void* esi_key) {

		return !is_executing(esi_key) && !is_in_ready(esi_key);
	}

	t_list* esis_that_cant_be_blocked_by_console = g_new_blocked_by_console_esis;

	if(esis_that_cant_be_blocked_by_console->elements_count != 0) {

		void show_esi_that_wasnt_blocked(void* esi_key) {

			esi_information* esi_inf = obtain_esi_information_by_id_test(((esi_sexpecting_key*)esi_key)->esi_fd, g_esi_bursts);

			if (esi_inf != NULL) {

				printf("El ESI %i no pudo bloquearse porque no se encuentra en ready ni tampoco en ejecución\n", esi_inf->esi_numeric_name);
			} else {

				printf("El ESI elegido no existe\n");

				if(!determine_if_key_is_blocked(((esi_sexpecting_key*)esi_key)->key, g_locked_keys)){

					printf("La clave no la tenía nadie, entonces la toma el sistema\n");
					list_add(g_locked_keys, (void*)create_key_blocker(((esi_sexpecting_key*)esi_key)->key, -1));
				}
			}
		}

		list_iterate(esis_that_cant_be_blocked_by_console, show_esi_that_wasnt_blocked);
	}

	void destroy_esi_sexpecting_key(void* esi_sexpecting_key_) {

		free(((esi_sexpecting_key*) esi_sexpecting_key_)->key);
		free(esi_sexpecting_key_);
	}

	list_clean_and_destroy_elements(g_new_blocked_by_console_esis, destroy_esi_sexpecting_key);

	printf("\n");
}

static esi_information* obtain_esi_information_by_id_test(int esi_fd, t_list* esi_bursts){

	bool equal_condition(void* esi_inf) {

		return ((esi_information*)esi_inf)->esi_id == esi_fd;
	}

	return list_find(esi_bursts, equal_condition);
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

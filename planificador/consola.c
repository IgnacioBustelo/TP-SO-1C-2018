#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>

#include "planificador.h"
#include "../libs/comando.h"
#include "../libs/serializador.h"
#include "../libs/deserializador.h"
#include "../protocolo/protocolo.h"
#include "../libs/conector.h"

#include "consola.h"

static void pause_scheduler(char **_);
static void resume_scheduler(char **_);
static void lock_process(char **args);
static void unlock_process(char **args);
static void list_locked_process(char **args);
static void kill_process(char **args);
static void key_status(char **args);
static void check_deadlock(char **_);

#define COMMANDS_SIZE (sizeof(commands) / sizeof(*commands))
#define eprintf(args...) fprintf (stderr, args)

int scheduler_paused_flag;
int block_esi_by_console_flag;
int unlock_esi_by_console_flag;
int killed_esi_flag;

t_list* g_last_unlocked_by_console_keys;
t_list* g_new_killed_esis;
t_list* g_esis_sexpecting_keys;
t_list* g_new_blocked_by_console_esis;
t_list* g_locked_keys;
t_list* g_esi_bursts;
sem_t mutex_coordinador;
int g_coordinator_fd;

static struct command_t commands[] = {
	DEF_COMMAND("pausar",      0, pause_scheduler),
	DEF_COMMAND("continuar",   0, resume_scheduler),
	DEF_COMMAND("bloquear",    2, lock_process),
	DEF_COMMAND("desbloquear", 1, unlock_process),
	DEF_COMMAND("listar",      1, list_locked_process),
	DEF_COMMAND("kill",        1, kill_process),
	DEF_COMMAND("status",      1, key_status),
	DEF_COMMAND("deadlock",    0, check_deadlock)
};

int execute_console_command(char *command_line)
{
	return execute_command_line(commands, COMMANDS_SIZE, command_line);
}

void *init_console(void* _)
{

	for (;;) {
		char *line = readline("> ");
		int execute_result = execute_console_command(line);
		switch (execute_result) {
		case NO_COMMAND_ERROR:
			eprintf("Comando invalido!\n");
			break;
		case ARGUMENT_COUNT_ERROR:
			eprintf("Numero de argumentos incorrecto\n");
			break;
		default:
			printf("Comando ejecutado correctamente\n");
			break;
		}
		free(line);
	}

	return NULL;
}

static void pause_scheduler(char **_)
{
	printf("Pausar planificacion\n");

	scheduler_paused_flag = 1;
}

static void resume_scheduler(char **_)
{
	printf("Continuar planificacion\n");

	scheduler_paused_flag = 0;
}

static void lock_process(char **args)
{
	void _lock_process(char *key, char *pid) {

		printf("Bloquear proceso ESI (clave = %s, id = %s)\n", key, pid);
		block_esi_by_console_flag = 1;

		int pfd = obtain_esi_fd_by_esi_pid(atoi(pid));

		list_add(g_new_blocked_by_console_esis, (void*)create_esi_sexpecting_key(pfd, key));
		free(key);
		free(pid);
	}
	_lock_process(args[0], args[1]);
}

static void unlock_process(char **args)
{
	void _unlock_process(char* key) {
		printf("Desbloquear proceso ESI (clave = %s)\n", key);
		unlock_esi_by_console_flag = 1;
		char* key_ = strdup(key);
		list_add(g_last_unlocked_by_console_keys, (void*)key_);
	}

	_unlock_process(args[0]);
}

static void list_locked_process(char **args)
{
	void _list_locked_process(char* resource) {

		printf("Listar procesos bloqueados por recurso %s\n", resource);
		show_blocked_process(resource);
	}
	_list_locked_process(args[0]);
}

static void kill_process(char **args)
{
	void _kill_process(char *pid) {
		printf("Finalizar proceso %s\n", pid);
		killed_esi_flag = 1;

		int* pfd = malloc(sizeof(int));
		*pfd = obtain_esi_fd_by_esi_pid(atoi(pid));

		if(*pfd == -1){

			printf("El ESI elegido para matar no existe. Busque otra victima\n");
		} else {

			list_add(g_new_killed_esis, (void*)pfd);
		}

		free(pid);
	}
	_kill_process(args[0]);
}

static void key_status(char **args)
{
	void _key_status(char *key) {
		printf("Informar estado de la clave %s\n", key);

		sem_wait(&mutex_coordinador);

		send_key_to_coordinator(key);

		receive_and_print_key_status();

		sem_post(&mutex_coordinador);

		show_blocked_process(key);
	}
	_key_status(args[0]);
}

static void check_deadlock(char **_)
{
	printf("Detectar deadlocks\n");
	detect_and_show_all_deadlocks(g_locked_keys, g_esis_sexpecting_keys, g_esi_bursts);
}

void show_blocked_process(char* resource) {

	void show_esi_from_resource(void* resource2){

		if (strcmp(((esi_sexpecting_key*)resource2)->key, resource) == 0) {

			esi_information* esi_inf = obtain_esi_information_by_id(((esi_sexpecting_key*)resource2)->esi_fd);
			printf("El ESI %i se encuentra bloqueado por la clave %s\n", esi_inf->esi_numeric_name, resource);
		}
	}

	list_iterate(g_esis_sexpecting_keys, show_esi_from_resource);
}

void detect_and_show_all_deadlocks(t_list* locked_keys, t_list* esi_requests, t_list* esis_in_system) {

	typedef struct esi_in_deadlock {

		int esi_id;
		int esi_state;
	}esi_in_deadlock;

	esi_in_deadlock* create_esi_in_deadlock(int esi_id_) {

		esi_in_deadlock* esi = malloc(sizeof(esi_in_deadlock));
		esi->esi_id = esi_id_;
		esi->esi_state = -1;
		return esi;
	}

	int* create_esi(int esi_id) {

		int* esi = malloc(sizeof(int));
		*esi = esi_id;
		return esi;
	}

	void destroy_esi_in_deadlock(void* esi) {

		free((esi_in_deadlock*)esi);
	}

	void int_destroyer(void* esi) {

		free((int*)esi);
	}

	t_list* all_esis_in_system = list_create();
	t_list* current_esi_cycle = list_create();

	int deadlock_number = 0;

	void add_all_esi_numbers(void* esi_inf) {

		int esi_id = ((esi_information*)esi_inf)->esi_id;
        esi_in_deadlock* esi = create_esi_in_deadlock(esi_id);

		list_add(all_esis_in_system, (void*)esi);
	}

	list_iterate(esis_in_system, add_all_esi_numbers);

	void put_a_zero_to_esis_that_are_not_holding_keys(void* esi) {

		bool esi_holding_a_key(int esi_id_) {

			bool holding_a_key(void* key_blocker_) {

			int esi_found_number = ((key_blocker*)key_blocker_)->esi_id;
			return esi_found_number == esi_id_;
			}

			return list_any_satisfy(locked_keys, holding_a_key);
		}

		if(!esi_holding_a_key(((esi_in_deadlock*)esi)->esi_id)){

			((esi_in_deadlock*)esi)->esi_state = 0;
		}
	}

	list_iterate(all_esis_in_system, put_a_zero_to_esis_that_are_not_holding_keys);

	bool esi_not_analized(void* esi) {

		return ((esi_in_deadlock*)esi)->esi_state == -1;
	}

	while(list_any_satisfy(all_esis_in_system, esi_not_analized)) {

		esi_in_deadlock* esi = list_find(all_esis_in_system, esi_not_analized);

		bool esi_asks_for_a_key(int esi_id_) {

			bool condition(void* esi_sexpecting) {

				return ((esi_sexpecting_key*)esi_sexpecting)->esi_fd == esi_id_;
			}

			return list_any_satisfy(esi_requests, condition);
		}

		if(!esi_asks_for_a_key(esi->esi_id)) {

			esi->esi_state = 0;
		} else {

			list_add(current_esi_cycle, (void*)create_esi(esi->esi_id));

			bool find_request(void* esi_sexpecting) {

				return ((esi_sexpecting_key*)esi_sexpecting)->esi_fd == esi->esi_id;
			}

			esi_sexpecting_key* esi_sexpecting = list_find(esi_requests, find_request);
			int esi_id;

			int obtain_key_owner(char* requested_key_) {

				bool esi_owner(void* key_locked) {

					return strcmp(((key_blocker*) key_locked)->key, requested_key_) == 0;
				}

				key_blocker* key_locked = list_find(locked_keys, esi_owner);
				return key_locked->esi_id;
			}

			esi_id = obtain_key_owner(esi_sexpecting->key);

			esi_sexpecting_key* obtain_esi_request(int esi_id) {

				bool find_request(void* esi_sexpecting) {

					return ((esi_sexpecting_key*) esi_sexpecting)->esi_fd == esi_id;
				}

				return list_find(esi_requests, find_request);
			}

			bool esi_requests_matches_first_esi_in_cycle_assigned_key(int esi_id) {

				int first_esi = *(int*)list_get(current_esi_cycle, 0);
				esi_sexpecting_key* esi_sexpecting = obtain_esi_request(esi_id);

				if(esi_sexpecting == NULL) return false;

				bool has_asked_key_and_it_is_the_first_one(void* esi_blocker) {

					return ((key_blocker*)esi_blocker)->esi_id == first_esi && strcmp(esi_sexpecting->key, ((key_blocker*)esi_blocker)->key) == 0;
				}

				return list_any_satisfy(locked_keys, has_asked_key_and_it_is_the_first_one);
			}

			list_add(current_esi_cycle, (void*)create_esi(esi_id));

			while(esi_asks_for_a_key(esi_id) && !esi_requests_matches_first_esi_in_cycle_assigned_key(esi_id)) {

				esi_sexpecting = obtain_esi_request(esi_id);
				esi_id = obtain_key_owner(esi_sexpecting->key);
				list_add(current_esi_cycle, (void*)create_esi(esi_id));
			}

			if(!esi_asks_for_a_key(esi_id)) {

				void apply_a_zero(void* esi) {

					bool find_condition(void* esi_in_deadlock_) {

						return ((esi_in_deadlock*) esi_in_deadlock_)->esi_id == *(int*) esi;
					}

					esi_in_deadlock* esi_ = list_find(all_esis_in_system, find_condition);
					esi_->esi_state = 0;
				}

				list_iterate(current_esi_cycle, apply_a_zero);

				list_clean_and_destroy_elements(current_esi_cycle, int_destroyer);

			} else {

				deadlock_number++;

				void apply_deadlock_id(void* esi) {

					bool find_condition(void* esi_in_deadlock_) {

						return ((esi_in_deadlock*) esi_in_deadlock_)->esi_id == *(int*) esi;
					}

					esi_in_deadlock* deadlocked_esi = list_find(all_esis_in_system, find_condition);
					deadlocked_esi->esi_state = deadlock_number;
				}

				list_iterate(current_esi_cycle, apply_deadlock_id);

				list_clean_and_destroy_elements(current_esi_cycle, int_destroyer);
			}
	}
}
	if(deadlock_number == 0) {

		printf("No hay deadlocks en el sistema\n");
	}

	int i;
	for(i = 1; i <= deadlock_number; i++) {

		printf("Deadlock %i:\n", i);

		void is_in_deadlock_i(void* esi_in_deadlock_) {

			if(((esi_in_deadlock*)esi_in_deadlock_)->esi_state == i) {

				esi_information* obtain_esi_information_by_id_(int esi_fd){

					bool equal_condition(void* esi_inf) {

						return ((esi_information*)esi_inf)->esi_id == esi_fd;
					}

					return list_find(esis_in_system, equal_condition);
				 }

				int esi_number = obtain_esi_information_by_id_(((esi_in_deadlock*)esi_in_deadlock_)->esi_id)->esi_numeric_name;
				printf("ESI %i\n", esi_number);
			}
		}

		list_iterate(all_esis_in_system, is_in_deadlock_i);
	}

	list_clean_and_destroy_elements(all_esis_in_system, destroy_esi_in_deadlock);
}

void send_key_to_coordinator(char* key) {

	protocol_id operation = PROTOCOL_PC_KEY_STATUS;
	size_t package_size = sizeof(operation) + sizeof(size_t) + strlen(key) + 1;
	package_t* package = create_package(package_size);
	add_content(package, &operation, sizeof(operation));
	add_content_variable(package, key, strlen(key) + 1);
	void* package_ = build_package(package);
	send_serialized_package(g_coordinator_fd, package_, package_size);
}

void receive_and_print_key_status() {

	protocol_id operation_code;
	int key_state;

	recv_package(g_coordinator_fd, &operation_code, sizeof(operation_code));

	if(operation_code != PROTOCOL_CP_KEY_STATUS) {

		printf("Algo salió mal en la comunicación con el coordinador\n"); // TODO Ver si hay que hacer algo en esta situación
	} else {

	recv_package(g_coordinator_fd, &key_state, sizeof(int));

	switch(key_state) {

	case -1:

		printf("La clave solicitada no existe, por lo que no tiene valor ni instancia\n");
		break;

	case 0: {

		printf("La clave existe pero no está asignada actualmente a una instancia\n");
		char* instance_name;
		recv_package_variable(g_coordinator_fd, (void**) &instance_name);
		printf("Instancia en la que se asignaría: %s\n", instance_name);
		break;
	}

	case 1: {

		char* instance_name;
		char* key_value;

		recv_package_variable(g_coordinator_fd, (void**) &instance_name);
		recv_package_variable(g_coordinator_fd, (void**) &key_value);

		printf("Valor: %s\n", key_value);
		printf("Instancia actual: %s\n", instance_name);
		break;
	}
	}
  }
}


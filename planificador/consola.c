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
		list_add(g_new_blocked_by_console_esis, (void*)create_esi_sexpecting_key(atoi(pid), key));

	}
	_lock_process(args[0], args[1]);
}

static void unlock_process(char **args)
{
	void _unlock_process(char* key) {
		printf("Desbloquear proceso ESI (clave = %s)\n", key);
		unlock_esi_by_console_flag = 1;
		last_unlocked_key_by_console = strdup(key);
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
		int esi_id = atoi(pid);
		list_add(g_new_killed_esis, (void*)&esi_id);
	}
	_kill_process(args[0]);
}

static void key_status(char **args)
{
	void _key_status(char *key) {
		printf("Informar estado de la clave %s\n", key);

		/* Probablemente se necesite un mutex para sincronizar la comunicación con el coordinador y que no explote TODO horrendamente */

		/* Hay que comunicarse con el coordinador para obtener los datos de la clave respecto de las instancias */

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

	void destroy_esi_in_deadlock(void* esi) {

		free((esi_in_deadlock*)esi);
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

			list_add(current_esi_cycle, (void*)&(esi->esi_id));

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

				bool esi_owner(void* esi_blocker) {

					return ((key_blocker*)esi_blocker)->esi_id == first_esi;
				}

				key_blocker* esi_blocker = list_find(locked_keys, esi_owner);

				return strcmp(esi_sexpecting->key, esi_blocker->key) == 0;
			}

			list_add(current_esi_cycle, (void*) &esi_id);

			while(esi_asks_for_a_key(esi_id) && !esi_requests_matches_first_esi_in_cycle_assigned_key(esi_id)) {

				esi_sexpecting = obtain_esi_request(esi_id);
				esi_id = obtain_key_owner(esi_sexpecting->key);
				list_add(current_esi_cycle, (void*)&esi_id);
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

				list_clean(current_esi_cycle);

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

				list_clean(current_esi_cycle);
			}
	}
}
	int i;
	for(i = 1; i <= deadlock_number; i++) {

		printf("Deadlock %i\n", i);

		void is_in_deadlock_i(void* esi_in_deadlock_) {

			if(((esi_in_deadlock*)esi_in_deadlock_)->esi_state == i) {

				int esi_number = obtain_esi_information_by_id(((esi_in_deadlock*)esi_in_deadlock_)->esi_id)->esi_numeric_name;
				printf("ESI %i\n", esi_number);
			}
		}

		list_iterate(all_esis_in_system, is_in_deadlock_i);

		printf("\n");
	}

	list_clean_and_destroy_elements(all_esis_in_system, destroy_esi_in_deadlock);
}



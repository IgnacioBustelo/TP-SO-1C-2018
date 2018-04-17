#include <stdio.h>
#include <stdbool.h>

#include "../libs/comando.h"
#include "consola.h"

static void pause_scheduler(void);
static void lock_process(char **args);
static void unlock_process(char **args);
static void list_locked_process(char **args);
static void kill_process(char **args);
static void instance_status(char **args);
static void check_deadlock(void);

static bool _check_arg_count(char **args, int expected_count);

#define COMMANDS_SIZE (sizeof(commands) / sizeof(*commands))

static struct command_t commands[] = {
	DEF_COMMAND           ("pausar",      pause_scheduler),
	DEF_COMMAND_WITH_ARGS ("bloquear",    lock_process),
	DEF_COMMAND_WITH_ARGS ("desbloquear", unlock_process),
	DEF_COMMAND_WITH_ARGS ("listar",      list_locked_process),
	DEF_COMMAND_WITH_ARGS ("kill",        kill_process),
	DEF_COMMAND_WITH_ARGS ("status",      instance_status),
	DEF_COMMAND           ("deadlock",    check_deadlock)
};

bool execute_console_command(char *command_line)
{
	return execute_command_line(commands, COMMANDS_SIZE, command_line);
}

static void pause_scheduler(void)
{
	printf("Pausar planificacion\n");
}

static void lock_process(char **args)
{
	void _lock_process(char *key, char *pid) {
		printf("Bloquear proceso ESI (clave = %s, id = %s)\n", key, pid);
	}
	if (_check_arg_count(args, 2)) {
		_lock_process(args[0], args[1]);
	}
}

static void unlock_process(char **args)
{
	void _unlock_process(char *key) {
		printf("Desbloquear proceso ESI (clave = %s)\n", key);
	}

	if (_check_arg_count(args, 1)) {
		_unlock_process(args[0]);
	}
}

static void list_locked_process(char **args)
{
	void _list_locked_process(char *resource) {
		printf("Listar processos bloqueados por recurso %s\n", resource);
	}
	if (_check_arg_count(args, 1)) {
		_list_locked_process(args[0]);
	}
}

static void kill_process(char **args)
{
	void _kill_process(char *pid) {
		printf("Finalizar proceso %s\n", pid);
	}
	if (_check_arg_count(args, 1)) {
		_kill_process(args[0]);
	}
}

static void instance_status(char **args)
{
	void _instance_status(char *key) {
		printf("Informar estado de la instancia %s\n", key);
	}
	if (_check_arg_count(args, 1)) {
		_instance_status(args[0]);
	}
}

static void check_deadlock(void)
{
	printf("Detectar deadlock\n");
}

static bool _check_arg_count(char **args, int expected_count)
{
	void _handle_error(void) {
		printf("Numero de argumentos incorrecto\n");
	}

	int i;
	for (i = 0; i < expected_count; i++) {
		if (args[i] == NULL) {
			_handle_error();
			return false;
		}
	}

	if (args[i] != NULL) {
		_handle_error();
		return false;
	}

	return true;
}

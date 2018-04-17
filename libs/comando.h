#ifndef PLANIFICADOR_CONSOLA_H_
#define PLANIFICADOR_CONSOLA_H_

#include <stdbool.h>

typedef void (*command_func)(void);
typedef void (*command_func_with_args)(char **);

struct command_t {
	char *name;
	bool has_args;
	command_func function;		// Si tiene argumentos, recibe un char**, si no es void.
};

/**
 * Parsea y ejecuta una linea de comando.
 *   - commands:      Set de commandos
 *   - commands_size: Tamanio de set de commandos
 *   - command_line:  Linea de commando a procesar
 */
bool execute_command_line(struct command_t *commands,
							 int commands_size,
							 const char *command_line);

#endif /* PLANIFICADOR_CONSOLA_H_ */

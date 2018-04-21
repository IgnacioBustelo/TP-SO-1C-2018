#ifndef COMANDO_H_
#define COMANDO_H_

#include <stdbool.h>
#include <stddef.h>

typedef void (*command_func)(char **);

#define DEF_COMMAND(NAME, N_ARGS, FUNC_NAME)		\
	{ NAME, N_ARGS, FUNC_NAME }

#define NO_COMMAND_ERROR		-1
#define ARGUMENT_COUNT_ERROR	-2

struct command_t {
	char *name;
	size_t n_args;				// Cantidad de argumentos.
	command_func function;		// Si tiene argumentos, recibe un char**, si no es void.
};

/**
 * Parsea y ejecuta una linea de comando.
 *   - commands:      Set de commandos
 *   - commands_size: Tamanio de set de commandos
 *   - command_line:  Linea de commando a procesar
 */
int execute_command_line(struct command_t *commands,
							int commands_size,
							char *command_line);

#endif /* COMANDO_H_ */

#include <stdlib.h>
#include <readline/readline.h>

#include "../../libs/comando.h"
#include "../consola.h"

#define eprintf(args...) fprintf (stderr, args)

int main(void)
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

	return EXIT_SUCCESS;
}

#include <stdlib.h>
#include <readline/readline.h>

#include "../consola.h"

int main(void)
{
	for (;;) {
		char *line = readline("> ");
		if (!execute_console_command(line)) {
			printf("Comando invalido!\n");
		}
		free(line);
	}

	return EXIT_SUCCESS;
}

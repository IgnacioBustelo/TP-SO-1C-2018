#ifndef PLANIFICADOR_CONSOLA_H_
#define PLANIFICADOR_CONSOLA_H_

#include <stdbool.h>

/**
 * Recibe una linea de comando y lo ejecuta.
 * El valor de retorno indica si es un comando valido.
 */
bool execute_console_command(char *command_line);

#endif /* PLANIFICADOR_CONSOLA_H_ */

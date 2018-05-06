#ifndef PLANIFICADOR_CONSOLA_H_
#define PLANIFICADOR_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include "../libs/comando.h"
#include "planificador.h"

extern int scheduler_paused_flag;
extern t_list* g_esis_sexpecting_keys;

/**
 * Recibe una linea de comando y lo ejecuta.
 * El valor de retorno indica si fue ejecutado exitosamente u ocurrio un error:
 *   -  (1) Exito.
 *   - (-1) Comando no existente.
 *   - (-2) Cantidad incorrecta de argumentos.
 */
int execute_console_command(char *command_line);
void *init_console();

#endif /* PLANIFICADOR_CONSOLA_H_ */


/*
 * Establece el flag de pausa en 1
 */
void pause_planific(void);
/*
 * Muestra los procesos bloqueados por una determinada key
 */
void show_blocked_process(void * resource);

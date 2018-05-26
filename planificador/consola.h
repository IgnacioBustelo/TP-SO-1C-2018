#ifndef PLANIFICADOR_CONSOLA_H_
#define PLANIFICADOR_CONSOLA_H_

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include "../libs/comando.h"
#include "../libs/serializador.h"
#include "../libs/deserializador.h"
#include "../protocolo/protocolo.h"
#include "../libs/conector.h"
#include "planificador.h"

/**
 * Recibe una linea de comando y lo ejecuta.
 * El valor de retorno indica si fue ejecutado exitosamente u ocurrio un error:
 *   -  (1) Exito.
 *   - (-1) Comando no existente.
 *   - (-2) Cantidad incorrecta de argumentos.
 */
int execute_console_command(char *command_line);

void *init_console();

/*
 * Muestra los procesos bloqueados por una determinada clave
 */

void show_blocked_process(char* resource);

/*
 * Se encarga de detectar todos los deadlocks que hay en el momento y los muestra
 */

void detect_and_show_all_deadlocks();

/*
 * Le envía la clave al coordinador para consultar su estado
 */

void send_key_to_coordinator(char* key);

/*
 * Recibe el paquete con la información de la clave solicitada al coordinador y muestra el estado de la misma
 */

void receive_and_print_key_status();

#endif /* PLANIFICADOR_CONSOLA_H_ */

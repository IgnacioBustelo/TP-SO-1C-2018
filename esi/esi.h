#ifndef ESI_ESI_H_
#define ESI_ESI_H_

#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdlib.h>
#include <parsi/parser.h>

#include "../protocolo/protocolo.h"

/*
 * Obtiene el paquete que tiene que enviar al coordinador a partir de una línea del archivo
 */

void* obtain_package_from_line(char* line, size_t* package_size);

/*
 * El ESI se bloquea esperando que el planificador le de la orden de ejecutar la próxima sentencia del script
 */

void wait_for_execution_order(int scheduler_fd);

/*
 * El ESI se queda bloqueado esperando el resultado de ejecución de parte del coordinador
 */

protocol_id wait_for_execution_result(int coordinador_fd_);

/*
 * Se finaliza el programa satisfactoriamente o no
 */

void exit_gracefully(int status);

#endif

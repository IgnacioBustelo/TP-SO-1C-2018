#ifndef ESI_CONFIG_H_
#define ESI_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/config.h>
#include <commons/log.h>
#include "esi.h"
#include <commons/string.h>

/* -- Data Structures --  */

typedef struct {
	char* ip_coordinador;
	int port_coordinador;
	char* ip_planificador;
	int port_planificador;
} t_esi_config;

/*
 * Crea el logger y lo devuelve
 */

t_log* init_log();

/*
 * Obtiene los datos necesarios del archivo de configuración y los carga en una estructura setup
 */

t_esi_config init_config();


#endif

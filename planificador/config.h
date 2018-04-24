#ifndef PLANIFICADOR_CONFIG_H_
#define PLANIFICADOR_CONFIG_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/config.h>
#include <commons/log.h>
#include "planificador.h"
#include <commons/string.h>

/* Tipo de algoritmos de planificación */

typedef enum { FIFO, SJFCD, SJFSD, HRRN } t_scheduling_algorithm;

/* Estructura necesaria para depositar toda la información del planificador */

typedef struct {
	int port;
	int coordinator_port;
	t_scheduling_algorithm scheduling_algorithm;
	double initial_estimation;
	char* coordinator_ip;
	char** blocked_keys;
} t_planificador_config;

/*
 * Inicia el log para el planificador
 */

t_log* init_log();

/*
 * Devuelve una estructura que contiene todos los campos necesarios para el planificador
 */

t_planificador_config init_config(t_log* logger);

#endif

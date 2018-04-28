#ifndef PLANIFICADOR_PLANIFICADOR_H_
#define PLANIFICADOR_PLANIFICADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>

#define MAXCONN 20

/*
 * Nodo de ESI que ha bloqueado alguna clave
 */

typedef struct key_blocker {

	char* key;
	int esi_id;
} key_blocker;

/*
 * Nodo de ESI que está en espera de una clave bloqueada
 */

typedef struct esi_sexpecting_key {

	int esi_fd;
	char* key;
} esi_sexpecting_key;

/*
 * Información del ESI necesaria para la planificación
 */

typedef struct esi_information {

	int esi_id;
	double last_estimated_burst;
	double next_left_estimated_burst;
	int last_real_burst;
	int waited_bursts;
} esi_information;

/*
 * Función para salir del programa según un fallo o un éxito
 */

void exit_gracefully(int status);

/*
 * Crea un puntero a un key_blocker
 */

key_blocker* create_key_blocker(char* key, int esi_id);

/*
 * Crea un puntero a un esi_sexpecting_key
 */

esi_sexpecting_key* create_esi_sexpecting_key(int esi_fd, char* key);

/*
 * Crea un esi_information
 */

esi_information* create_esi_information(int esi_id);

/*
 * Inicializa la lista de esi's y la de claves bloqueadas
 */

void create_administrative_structures();

/*
 * Destruye la lista de esi's y la de claves bloqueadas
 */

void destroy_administrative_structures();

/*
 * Agrega a un esi a la cola de nuevos
 */

void put_new_esi_on_new_queue(int new_client_fd);

/*
 * Todos los ESI's en ready aumentan en uno su tiempo esperado
 */

void update_waiting_time_of_ready_esis();

/*
 * Planifica todos los esis que se encuentren en la lista de ready
 */

int schedule_esis();

/*
 * Borra al esi de la primer lista y lo agrega a la otra
 */

void move_esi_from_and_to_queue(t_list* from_queue, t_list* to_queue, int esi_fd);

/*
 * Envía señal de ejecución al ESI
 */

void authorize_esi_execution(int esi_fd);

/*
 * El ESI se encuentra en ready
 */

bool esi_information_in_ready(esi_information* esi_inf);

/*
 * Le aumenta uno a la ráfaga real ejecutada y le resta uno al tiempo estimado actual
 */

void update_executing_esi(int esi_fd);

/*
 * Devuelve el resultado de la ejecución de parte de un ESI. Podría pasar que esté todo bien, que se haya bloqueado o que falle por alguna razón
 */

int receive_execution_result(int esi_fd);

/*
 * Dice si determinada clave ya fue tomada por un ESI (hizo un GET)
 */

bool determine_if_key_is_blocked(char* blocked_key);

/*
 * Dice si la clave solicitada la bloqueó el ESI en ejecución
 */

bool key_is_blocked_by_executing_esi(char* key);

/*
 * Toma a los ESI's que estaban bloqueados por la clave desbloqueada de la lista de sexpectantes, los elimina de ella y devuelve sus respectivos file descriptors
 */

t_list* unlock_esis(char* key_unlocked);

/*
 * Desbloquea a los ESI's bloqueados por la clave recientemente desbloqueada
 */

void update_blocked_esi_queue(char* last_key_inquired, int* update_blocked_esi_queue_flag);

/*
 * Replanifica la cola de ready
 */

void reschedule(int* reschedule_flag, int* old_executing_esi);

/*
 * Mueve todos los ESI's nuevos a ready y agrega un nodo por cada ESI a la lista de esi_bursts
 */

void update_new_esi_queue(int* new_esi_flag);

/*
 * Libera todos los recursos que tenía alocados el ESI que finalizó su script
 */

void release_resources(int esi_fd, int* update_blocked_esi_queue_flag);

#endif /* PLANIFICADOR_PLANIFICADOR_H_ */

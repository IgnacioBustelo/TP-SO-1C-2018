#ifndef PLANIFICADOR_PLANIFICADOR_H_
#define PLANIFICADOR_PLANIFICADOR_H_

#define execute_order 3

typedef struct key_blocker {

	char* key;
	int esi_id;
} key_blocker;

typedef struct esi_information {

	int esi_id;
	double last_estimated_burst;
	double next_left_estimated_burst;
	int last_real_burst;
	int waited_bursts;
} esi_information;

void init_log();

void init_config();

void check_config(char* key);

void set_distribution(char* algorithm_name);

void exit_gracefully(int status);

/*
 * Crea un puntero a un key_blocker
 */

key_blocker* create_key_blocker(char* key, int esi_id);

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
 * Agrega a un esi a la cola de listos
 */

void put_new_esi_on_ready_queue(int new_client_fd);

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
 * El planificador recibe la confirmación de fin de una rafaga del esi
 */

int receive_confirmation_from_esi(int fd);

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
 * Dice si determinada clave ya fue tomada por un ESI
 */

bool determine_if_key_is_blocked(char* blocked_key);

#endif /* PLANIFICADOR_PLANIFICADOR_H_ */

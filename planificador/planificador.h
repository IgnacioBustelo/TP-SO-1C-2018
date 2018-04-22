#ifndef PLANIFICADOR_PLANIFICADOR_H_
#define PLANIFICADOR_PLANIFICADOR_H_

#define execute_order 2

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
 * Destruye un key_blocker
 */

void destroy_key_blocker(void* key_blocker_);

/*
 * Crea un esi_information
 */

esi_information* create_esi_information(int esi_id);

/*
 * Destruye un esi_information
 */

void destroy_esi_information(void* esi_inf);

/*
 * Inicializa la lista de esi's y la de claves bloqueadas
 */

void create_administrative_structures();

/*
 * Destruye la lista de esi's y la de claves bloqueadas
 */

void destroy_administrative_structures();

/*
 * Echa al socket de fd_set, lo loggea y cierra el socket
 */

void remove_fd(int fd, fd_set *fdset);

/*
 * Agrega a un esi a la cola de listos
 */

void put_esi_on_ready_queue(new_client_fd);

/*
 * Le suma 1 al campo de ráfagas esperadas de un ESI
 */

void update_waited_bursts(esi_information* esi_inf);

/*
 * Planifica todos los esis que se encuentren en la lista de ready
 */

int schedule_esis();

//fifo for prob

/*
 * Envía señal de ejecución al ESI
 */

void authorize_esi_execution(int esi_fd);

/*
 * El planificador recibe la confirmación de fin de una rafaga del esi
 */

int receive_confirmation_from_esi(int fd);

/*
 * Indica que es necesario una planificación
 */

void we_must_reschedule(int* flag);

/*
 * Checkea si el algoritmo es con desalojo
 */
bool alg_is_non_preemptive();

#endif /* PLANIFICADOR_PLANIFICADOR_H_ */

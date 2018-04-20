#ifndef PLANIFICADOR_PLANIFICADOR_H_
#define PLANIFICADOR_PLANIFICADOR_H_

typedef struct key_blocker {

	char* key;
	int esi_id;
} key_blocker;

typedef struct esi_information {

	int esi_id;
	float next_burst;
	float last_burst;
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

#endif /* PLANIFICADOR_PLANIFICADOR_H_ */

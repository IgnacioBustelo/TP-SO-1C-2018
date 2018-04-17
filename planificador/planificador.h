#ifndef PLANIFICADOR_PLANIFICADOR_H_
#define PLANIFICADOR_PLANIFICADOR_H_

typedef struct key_blocker {

	char* key;
	int esi_id;
};

typedef struct esi_information {

	int esi_id;
	float next_burst;
	float last_burst;
};

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

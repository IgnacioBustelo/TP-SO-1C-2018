#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <commons/collections/list.h>
#include "../libs/conector.h"
#include "defines.h"

#include "coordinador.h"
#include "config.h"
#include "logger.h"
#include "instance-list/instance-list.h"
#include "connection/esi-connection.h"
#include "connection/instance-connection.h"
#include "connection/scheduler-connection.h"

/* Global variables */
t_log *logger;
struct setup_t setup;

/* Local functions */
static void init_server(int port);
static void *handle_connection(void *arg);
static int synchronize_connection(enum process_type type);

int main(void)
{
	init_config();
	init_server(setup.port);

	exit_gracefully(EXIT_SUCCESS);
}

static void init_server(int port)
{
	int listener_fd = init_listener(port, 10);

	if (listener_fd < 0) {
		switch (listener_fd) {
		case NO_FD_ERROR:
			log_error(logger, "No hay file descriptor disponible para el listener.");
			break;
		case BIND_ERROR:
			log_error(logger, "Error al intentar bindear.");
			break;
		case LISTEN_ERROR:
			log_error(logger, "Error al intentar crear cola de escucha.");
			break;
		}

		exit_gracefully(EXIT_FAILURE);

	} else {
		log_info(logger, "Escuchando en el socket %d", listener_fd);

		struct sockaddr_in client_info;
		socklen_t addrlen = sizeof client_info;

		pthread_attr_t attrs;
		pthread_attr_init(&attrs);
		pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);

		for (;;) {
			int *accepted_fd = malloc(sizeof(int));
			*accepted_fd = accept(listener_fd, (struct sockaddr *) &client_info, &addrlen);

			log_info(logger, "Creando un hilo para atender una conexión en el socket %d", *accepted_fd);

			pthread_t tid;
			pthread_create(&tid, &attrs, handle_connection, accepted_fd);
		}

		pthread_attr_destroy(&attrs);
	}
}

static void *handle_connection(void *arg)
{
	int fd = *((int *)arg);
	free(arg);

	int type = receive_handshake(fd);
	if (type < 0) {
		log_error(logger, "Socket %d: Error en el handshake.", fd);
		log_info(logger, "Socket %d: Cerrando conexion...", fd);
		close(fd);
		return NULL;
	}

	int retcode = synchronize_connection(type);
	if (retcode == -1) {
		log_error(logger, "Ya hay un Planificador conectado!");
	} else if (retcode == -2) {
		log_error(logger, "Se debe conectar primero el Planificador!");
	}

	bool confirmation = retcode == 1;
	if (!send_confirmation(fd, confirmation)) {
		log_error(logger, "Socket %d: Error al enviar confirmacion.", fd);
		log_info(logger, "Socket %d: Cerrando conexion...", fd);
		close(fd);
		return NULL;
	}

	if (confirmation) {
		switch (type) {
		case SCHEDULER:
			log_info(logger, "Socket %d identificado como Planificador", fd);
			handle_scheduler_connection(fd);
			break;
		case ESI:
			log_info(logger, "Socket %d identificado como ESI", fd);
			handle_esi_connection(fd);
			break;
		case INSTANCE:
			log_info(logger, "Socket %d identificado como Instancia", fd);
			handle_instance_connection(fd);
			break;
		default:
			log_error(logger, "Socket %d: Cliente intruso.", fd);
			break;
		}
	}

	log_info(logger, "Socket %d: Cerrando conexion...", fd);
	close(fd);

	return NULL;
}

static int synchronize_connection(enum process_type type)
{
	static bool scheduler_connected = false;
	static pthread_mutex_t scheduler_connected_mutex = PTHREAD_MUTEX_INITIALIZER;

	int retcode;

	synchronized(scheduler_connected_mutex) {
		if (type == SCHEDULER) {
			retcode = scheduler_connected ? -1 : 1;
			scheduler_connected = true;
		} else {
			retcode = scheduler_connected ? 1 : -2;
		}
	}

	return retcode;
}

void exit_gracefully(int status)
{
	exit(status);
}

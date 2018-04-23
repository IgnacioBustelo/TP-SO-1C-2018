#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "coordinador.h"
#include "config.h"
#include "../libs/conector.h"

/* Global variables */
t_log *logger;
struct setup_t setup;

/* Local functions */
static void init_log();
static void init_server(int port);
static void *handle_connection(void *arg);
static bool is_valid_process(int type);

int main(void)
{
	init_log();
	init_config();
	init_server(setup.port);

	exit_gracefully(EXIT_SUCCESS);
}

static void init_log()
{
	logger = log_create("coordinador.log", "Coordinador", true, LOG_LEVEL_INFO);
	log_info(logger, "Se inicio el logger.");
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
	bool confirmation = is_valid_process(type);

	send_confirmation(fd, confirmation);

	switch (type) {
	case SCHEDULER:
		// handle_scheduler_connection();
		break;
	case ESI:
		// handle_esi_connection();
		break;
	case INSTANCE:
		// handle_instance_connection();
		break;
	default:
		// fruta();
		return NULL;
	}

	return NULL;
}

static bool is_valid_process(int type)
{
	return (type == SCHEDULER || type == ESI || type == INSTANCE);
}

void exit_gracefully(int status)
{
	log_info(logger, "Finalizo la ejecucion del Coordinador.");
	log_destroy(logger);

	exit(status);
}

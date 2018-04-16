#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>

#include "../libs/conector.h"

int main(void) {

	logger = log_new("servidor.log");

	/* TODO Obtengo la ip y el puerto del coordinador del archivo de configuracion*/
	char* host;
	int port_coordinator;
	int server_port;

	int coordinator_fd = connect_to_server(host, port_coordinator);
	send_handshake(coordinator_fd, SCHEDULER);

	/*TODO Falta comprobar que el handshake haya sido exitoso*/

	int listener = init_listener(server_port, 10);

	fd_set connected_fds;
	fd_set read_fds;

	FD_ZERO(&connected_fds);
	FD_ZERO(&read_fds);
	FD_SET(coordinator_fd, &connected_fds);
	FD_SET(listener, &connected_fds);

	int max_fd = listener;

	while(1)
	{
		read_fds = connected_fds;

		if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
					log_error(logger, "Select error");
					exit(EXIT_FAILURE);
		}

		int fd;

		for (fd = 0; fd <= max_fd; fd++) {

					if (FD_ISSET(fd, &read_fds) == 0) {
						continue;

					} else if (fd == listener) {

						struct sockaddr_in client_info;
						socklen_t addrlen = sizeof client_info;
						log_info(logger, "New client connecting...");

						int new_client_fd = accept(listener, (struct sockaddr *) &client_info, &addrlen);

						if (new_client_fd == -1) {

							log_error(logger, "Accept error");
						} else {

							FD_SET(new_client_fd, &connected_fds);

							if (new_client_fd > max_fd) {

								max_fd = new_client_fd;
							}

							log_info(logger, "Socket %d connected", new_client_fd);

							if (receive_handshake(new_client_fd) == -1) {

								remove_fd(new_client_fd, &connected_fds);
								log_error(logger, "Handshake fail with socket %d", new_client_fd);
							}

						}

					} else if(fd == coordinator_fd){

						/* TODO -- El coordinador se comunica con el planificador*/

					}
				}

	}

	return EXIT_SUCCESS;
}

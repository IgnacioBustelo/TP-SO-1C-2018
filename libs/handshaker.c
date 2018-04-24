#include <commons/log.h>
#include <stdlib.h>

#include "handshaker.h"
#include "serializador.h"

#define VERIFICATION_NUMBER 151418

static int client_send_handshake(int fd, enum process_type type) {
	int verification = VERIFICATION_NUMBER, package_size = sizeof(verification) + sizeof(type), status;
	package_t* package;

	package = create_package(package_size);

	add_content(package, &verification, sizeof(verification));
	add_content(package, &type, sizeof(type));

	status = send(fd, build_package(package), package_size, 0);

	destroy_package(package);

	if (status != package_size) {
		return 0;
	}

	else {
		return 0;
	}
}

static int client_receive_confirmation(int fd, bool *confirm) {
	if (recv(fd, confirm, sizeof(*confirm), MSG_WAITALL) == sizeof(*confirm)) {
		return 1;
	}

	else {
		return 0;
	}
}

int handshake_client(int server_fd, char* server_name, enum process_type type, t_log* logger) {

	bool confirmation;
	int status;

	status = client_send_handshake(server_fd, type);

	if (status != 1) {
		log_error(logger, "Fallo enviando solicitud de Handshake con %d", server_name);
		close(server_fd);
		return EXIT_FAILURE;
	}

	status = client_receive_confirmation(server_fd, &confirmation);

	if (status == -1) {
		log_error(logger, "Fallo en la conexion con %d", server_name);
		close(server_fd);
		return EXIT_FAILURE;
	}

	else if (!confirmation) {
		log_error(logger, "Verificacion invalida");
		close(server_fd);
		return EXIT_FAILURE;
	}

	else {
		log_info(logger, "Handshake satisfactorio con %d", server_name);
		return EXIT_SUCCESS;
	}

}

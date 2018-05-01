#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "../../protocolo/protocolo.h"
#include "../logger.h"

#include "esi-connection.h"

enum esi_operation_type_t {
	ESI_GET, ESI_SET, ESI_STORE
};

struct esi_operation_t {
	enum esi_operation_type_t type;
	union {
		struct {
			char* key;
		} get;
		struct {
			char* key;
			char* value;
		} set;
		struct {
			char* key;
		} store;
	};
};

static struct esi_operation_t *recv_esi_operation(int fd);
static bool recv_esi_get_args(int fd, struct esi_operation_t *operation);
static bool recv_esi_set_args(int fd, struct esi_operation_t *operation);
static bool recv_esi_store_args(int fd, struct esi_operation_t *operation);

t_log *logger;

void handle_esi_connection(int fd)
{
	struct esi_operation_t *operation;
	for (operation = recv_esi_operation(fd); operation != NULL; operation = recv_esi_operation(fd)) {
		/* TODO:
		 * --receive request--
		 * struct instance_t *next_instance = equitative_load(instance_list);
		 * if (next_instance != NULL) {
		 *     // Mutex? (next_instance could be being removed from the instance_list)
		 *     instance_list_request_add(next_instance, --REQUEST--);	// <- Mutex inside this function
		 *     sem_post(&next_instance->requests_count);
		 * } else {
		 *     HANDLE_ERROR
		 * }
		 */

		/* ALTERNATIVE:
		 *   - Add request to request pool.
		 */
	}

	log_error(logger, "[ESI] Socket %d: Cerrando conexion.", fd);
}

static struct esi_operation_t *recv_esi_operation(int fd)
{
	protocol_id op_id;
	if (CHECK_RECV(fd, &op_id)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir operacion!", fd);
		return NULL;
	}

	struct esi_operation_t *operation = malloc(sizeof(struct esi_operation_t));
	bool success;

	switch (op_id) {
	case PROTOCOL_EC_GET:
		operation->type = ESI_GET;
		log_info(logger, "[ESI] Socket %d: Codigo de operacion GET recibida.", fd);
		success = recv_esi_get_args(fd, operation);
		break;
	case PROTOCOL_EC_SET:
		operation->type = ESI_SET;
		log_info(logger, "[ESI] Socket %d: Codigo de operacion SET recibida.", fd);
		success = recv_esi_set_args(fd, operation);
		break;
	case PROTOCOL_EC_STORE:
		operation->type = ESI_STORE;
		log_info(logger, "[ESI] Socket %d: Codigo de operacion recibida.", fd);
		success = recv_esi_store_args(fd, operation);
		break;
	default:
		log_error(logger, "[ESI] Socket %d: Operacion invalida!", fd);
		free(operation);
		return NULL;
	}

	if (success) {
		log_info(logger, "[ESI] Socket %d: Se recibio correctamente los argumentos de la operacion.", fd);
		return operation;
	} else {
		return NULL;
	}
}

static bool recv_esi_get_args(int fd, struct esi_operation_t *operation)
{
	size_t key_size;
	if (CHECK_RECV(fd, &key_size)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir tamanio de la clave!", fd);
		return false;
	}

	operation->get.key = malloc(key_size);
	if (CHECK_RECV_WITH_SIZE(fd, operation->get.key, key_size)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir la clave!", fd);
		free(operation->get.key);
		return false;
	}

	return true;
}

static bool recv_esi_set_args(int fd, struct esi_operation_t *operation)
{
	size_t key_size;
	if (CHECK_RECV(fd, &key_size)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir tamanio de la clave!", fd);
		return false;
	}

	operation->set.key = malloc(key_size);
	if (CHECK_RECV_WITH_SIZE(fd, operation->set.key, key_size)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir la clave!", fd);
		free(operation->set.key);
		return false;
	}

	size_t value_size;
	if (CHECK_RECV(fd, &value_size)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir tamanio del valor!", fd);
		return false;
	}

	operation->set.value = malloc(value_size);
	if (CHECK_RECV_WITH_SIZE(fd, operation->set.value, value_size)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir el valor!", fd);
		free(operation->set.key);
		free(operation->set.value);
		return false;
	}

	return true;
}

static bool recv_esi_store_args(int fd, struct esi_operation_t *operation)
{
	size_t key_size;
	if (CHECK_RECV(fd, &key_size)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir tamanio de la clave!", fd);
		return false;
	}

	operation->store.key = malloc(key_size);
	if (CHECK_RECV_WITH_SIZE(fd, operation->store.key, key_size)) {
		log_error(logger, "[ESI] Socket %d: Error al recibir la clave!", fd);
		free(operation->store.key);
		return false;
	}

	return true;
}

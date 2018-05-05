#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "../../protocolo/protocolo.h"
#include "../defines.h"
#include "../logger.h"

#include "../distribution.h"
#include "../instance-list/instance-request-list.h"
#include "instance-connection.h"
#include "esi-connection.h"
#include "scheduler-connection.h"

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

t_log *logger;
struct instance_list_t *instance_list;

static void handle_esi_operation(struct esi_operation_t *operation);
static struct esi_operation_t *recv_esi_operation(int fd);
static bool recv_esi_get_args(int fd, struct esi_operation_t *operation);
static bool recv_esi_set_args(int fd, struct esi_operation_t *operation);
static bool recv_esi_store_args(int fd, struct esi_operation_t *operation);

void handle_esi_connection(int fd)
{
	struct esi_operation_t *operation;
	for (operation = recv_esi_operation(fd); operation != NULL; operation = recv_esi_operation(fd)) {
		handle_esi_operation(operation);
	}

	log_error(logger, "[ESI] Socket %d: Cerrando conexion.", fd);
}

static void handle_esi_operation(struct esi_operation_t *operation)
{
	/* TODO: Log de operaciones. */
	if (operation->type == ESI_GET) {
		log_info(logger, "GET %s", operation->get.key);
		switch (scheduler_recv_key_state(operation->get.key)) {
		case KEY_UNBLOCKED:
		case KEY_BLOCKED_BY_EXECUTING_ESI:
			scheduler_block_key();
			break;
		case KEY_BLOCKED:
			/* TODO: Send YOU_ARE_BLOCKED */
			break;
		case KEY_RECV_ERROR:
			break;
		}
	} else {
		struct instance_t *instance = equitative_load(instance_list);
		if (instance == NULL) {
			// TODO: HANDLE_ERROR
			return;
		}
		if (operation->type == ESI_SET) {
			log_info(logger, "SET %s \"%s\"", operation->set.key, operation->set.value);
			request_list_push_set(instance->requests, operation->set.key, operation->set.value);
		} else /* operation->type == ESI_STORE */ {
			log_info(logger, "STORE %s", operation->store.key);
			request_list_push_store(instance->requests, operation->store.key);
		}
	}
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

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

static bool handle_esi_operation(int fd, struct esi_operation_t *operation);
static struct esi_operation_t *esi_recv_operation(int fd);
static bool esi_recv_get_args(int fd, struct esi_operation_t *operation);
static bool esi_recv_set_args(int fd, struct esi_operation_t *operation);
static bool esi_recv_store_args(int fd, struct esi_operation_t *operation);
static void esi_operation_destroy(struct esi_operation_t *victim);

void handle_esi_connection(int fd)
{
	struct esi_operation_t *operation;
	for (operation = esi_recv_operation(fd); operation != NULL; operation = esi_recv_operation(fd)) {
		if (!handle_esi_operation(fd, operation)) {
			esi_operation_destroy(operation);
			break;
		}
		esi_operation_destroy(operation);
	}

	log_error(logger, "[ESI] Socket %d: Cerrando conexion.", fd);
}

static bool handle_esi_operation(int fd, struct esi_operation_t *operation)
{
	/* TODO: Log de operaciones. */
	if (operation->type == ESI_GET) {
		log_info(logger, "GET %s", operation->get.key);
		switch (scheduler_recv_key_state(operation->get.key)) {
		case KEY_UNBLOCKED:
		case KEY_BLOCKED_BY_EXECUTING_ESI:
			log_info(logger, "[ESI] Socket %d: Bloqueando la clave...", fd);
			scheduler_block_key();
			esi_send_execution_success(fd);
			break;
		case KEY_BLOCKED:
			log_info(logger, "[ESI] Socket %d: Clave bloqueada.", fd);
			esi_send_notify_block(fd);
			break;
		case KEY_RECV_ERROR:
			log_error(logger, "[ESI] Socket %d: Error al recibir estado de la clave desde el Planificador!", fd);
			return false;
		}
	} else {
		struct instance_t *instance = equitative_load(instance_list);
		if (instance == NULL) {
			// TODO: HANDLE_ERROR
			log_error(logger, "No hay instancias disponibles!");
			return false;
		}
		if (operation->type == ESI_SET) {
			log_info(logger, "SET %s \"%s\"", operation->set.key, operation->set.value);
			request_list_push_set(instance->requests, fd, operation->set.key, operation->set.value);
		} else /* operation->type == ESI_STORE */ {
			log_info(logger, "STORE %s", operation->store.key);
			request_list_push_store(instance->requests, fd, operation->store.key);
		}
	}

	return true;
}

static struct esi_operation_t *esi_recv_operation(int fd)
{
	log_info(logger, "[ESI] Socket %d: Esperando una operacion de ESI...", fd);

	int op_id;
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
		success = esi_recv_get_args(fd, operation);
		break;
	case PROTOCOL_EC_SET:
		operation->type = ESI_SET;
		log_info(logger, "[ESI] Socket %d: Codigo de operacion SET recibida.", fd);
		success = esi_recv_set_args(fd, operation);
		break;
	case PROTOCOL_EC_STORE:
		operation->type = ESI_STORE;
		log_info(logger, "[ESI] Socket %d: Codigo de operacion recibida.", fd);
		success = esi_recv_store_args(fd, operation);
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
		free(operation);
		return NULL;
	}
}

static bool esi_recv_get_args(int fd, struct esi_operation_t *operation)
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

static bool esi_recv_set_args(int fd, struct esi_operation_t *operation)
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

static bool esi_recv_store_args(int fd, struct esi_operation_t *operation)
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

bool esi_send_execution_success(int fd)
{
	int msg = PROTOCOL_CE_EVERYTHING_OK;
	return CHECK_SEND(fd, &msg);
}

bool esi_send_notify_block(int fd)
{
	int msg = PROTOCOL_CE_YOU_ARE_BLOCKED;
	return CHECK_SEND(fd, &msg);
}

bool esi_send_illegal_operation(int fd)
{
	int msg = PROTOCOL_CE_ILLEGAL_OPERATION;
	return CHECK_SEND(fd, &msg);
}

static void esi_operation_destroy(struct esi_operation_t *victim)
{
	switch (victim->type) {
	case ESI_GET:
		free(victim->get.key);
		break;
	case ESI_SET:
		free(victim->set.key);
		free(victim->set.value);
		break;
	case ESI_STORE:
		free(victim->store.key);
		break;
	}

	free(victim);
}

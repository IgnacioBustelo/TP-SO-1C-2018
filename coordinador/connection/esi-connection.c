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

struct esi_t {
	int id;
	int fd;
};

t_log *logger;
struct instance_list_t *instance_list;

static bool handle_esi_operation(struct esi_t esi, struct esi_operation_t *operation);
static struct esi_operation_t *esi_recv_operation(struct esi_t esi);
static bool esi_recv_get_args(struct esi_t esi, struct esi_operation_t *operation);
static bool esi_recv_set_args(struct esi_t esi, struct esi_operation_t *operation);
static bool esi_recv_store_args(struct esi_t esi, struct esi_operation_t *operation);
static void esi_operation_destroy(struct esi_operation_t *victim);
static int esi_get_id(void);

t_log *operation_logger;
__attribute__((constructor)) void operation_logger_init(void) {
	operation_logger = log_create("operaciones.log", "Coordinador", true, LOG_LEVEL_INFO);
}

__attribute__((destructor)) void operation_logger_destroy(void) {
	log_destroy(operation_logger);
}

void handle_esi_connection(int fd)
{
	struct esi_t esi;
	esi.fd = fd;
	esi.id = esi_get_id();

	log_info(logger, "[ESI] Socket %d: ID = %d", esi.fd, esi.id);

	struct esi_operation_t *operation;
	for (operation = esi_recv_operation(esi); operation != NULL; operation = esi_recv_operation(esi)) {
		if (!handle_esi_operation(esi, operation)) {
			esi_operation_destroy(operation);
			break;
		}
		esi_operation_destroy(operation);
	}

	log_error(logger, "[ESI %d] Finalizando comunicacion...", esi.id);
}

#define esi_log_info(logger, fmt, args...) log_info(logger, "[ESI %d] " fmt, esi.id, ##args)
#define esi_log_error(logger, fmt, args...) log_error(logger, "[ESI %d] " fmt, esi.id, ##args)

#define esi_log_operation(fmt, args...) log_info(operation_logger, "ESI %d\t" fmt, esi.id, ##args)

static bool handle_esi_operation(struct esi_t esi, struct esi_operation_t *operation)
{
	/* TODO:
	 *   - Manejar desconexion del planificador.
	 */
	if (operation->type == ESI_GET) {
		esi_log_operation("GET %s", operation->get.key);
		switch (scheduler_recv_key_state(operation->get.key)) {
		case KEY_UNBLOCKED:
		case KEY_BLOCKED_BY_EXECUTING_ESI:
			esi_log_info(logger, "Bloqueando la clave...");
			scheduler_block_key();
			esi_send_execution_success(esi.fd);
			break;
		case KEY_BLOCKED:
			esi_log_info(logger, "Clave bloqueada.");
			esi_send_notify_block(esi.fd);
			break;
		}
	} else {
		char *key = operation->type == ESI_SET ? operation->set.key : operation->store.key;

		esi_log_info(logger, "Seleccionando una Instancia para ejecutar la operacion...");
		struct instance_t *instance = equitative_load(instance_list, key);
		if (instance == NULL) {
			/* TODO: Bloquear el ESI. */
			return false;
		}
		if (operation->type == ESI_SET) {
			esi_log_operation("SET %s \"%s\"", operation->set.key, operation->set.value);
			request_list_push_set(instance->requests, esi.fd, operation->set.key, operation->set.value);
		} else /* operation->type == ESI_STORE */ {
			esi_log_operation("STORE %s", operation->store.key);
			request_list_push_store(instance->requests, esi.fd, operation->store.key);
		}
	}

	return true;
}

static struct esi_operation_t *esi_recv_operation(struct esi_t esi)
{
	esi_log_info(logger, "Esperando una operacion de ESI...");

	int op_id;
	if (!CHECK_RECV(esi.fd, &op_id)) {
		esi_log_info(logger, "Finaliza la ejecucion del ESI.");
		return NULL;
	}

	struct esi_operation_t *operation = malloc(sizeof(struct esi_operation_t));
	bool success;

	switch (op_id) {
	case PROTOCOL_EC_GET:
		operation->type = ESI_GET;
		esi_log_info(logger, "Codigo de operacion GET recibida.");
		success = esi_recv_get_args(esi, operation);
		break;
	case PROTOCOL_EC_SET:
		operation->type = ESI_SET;
		esi_log_info(logger, "Codigo de operacion SET recibida.");
		success = esi_recv_set_args(esi, operation);
		break;
	case PROTOCOL_EC_STORE:
		operation->type = ESI_STORE;
		esi_log_info(logger, "Codigo de operacion recibida.");
		success = esi_recv_store_args(esi, operation);
		break;
	default:
		esi_log_error(logger, "Operacion invalida!");
		free(operation);
		return NULL;
	}

	if (success) {
		esi_log_info(logger, "Se recibio correctamente los argumentos de la operacion.");
		return operation;
	} else {
		free(operation);
		return NULL;
	}
}

static bool esi_recv_get_args(struct esi_t esi, struct esi_operation_t *operation)
{
	size_t key_size;
	if (!CHECK_RECV(esi.fd, &key_size)) {
		esi_log_error(logger, "Error al recibir tamanio de la clave!");
		return false;
	}

	operation->get.key = malloc(key_size);
	if (!CHECK_RECV_WITH_SIZE(esi.fd, operation->get.key, key_size)) {
		esi_log_error(logger, "Error al recibir la clave!");
		free(operation->get.key);
		return false;
	}

	return true;
}

static bool esi_recv_set_args(struct esi_t esi, struct esi_operation_t *operation)
{
	size_t key_size;
	if (!CHECK_RECV(esi.fd, &key_size)) {
		esi_log_error(logger, "Error al recibir tamanio de la clave!");
		return false;
	}

	operation->set.key = malloc(key_size);
	if (!CHECK_RECV_WITH_SIZE(esi.fd, operation->set.key, key_size)) {
		esi_log_error(logger, "Error al recibir la clave!");
		free(operation->set.key);
		return false;
	}

	size_t value_size;
	if (!CHECK_RECV(esi.fd, &value_size)) {
		esi_log_error(logger, "Error al recibir tamanio del valor!");
		return false;
	}

	operation->set.value = malloc(value_size);
	if (!CHECK_RECV_WITH_SIZE(esi.fd, operation->set.value, value_size)) {
		esi_log_error(logger, "Error al recibir el valor!");
		free(operation->set.key);
		free(operation->set.value);
		return false;
	}

	return true;
}

static bool esi_recv_store_args(struct esi_t esi, struct esi_operation_t *operation)
{
	size_t key_size;
	if (!CHECK_RECV(esi.fd, &key_size)) {
		esi_log_error(logger, "Error al recibir tamanio de la clave!");
		return false;
	}

	operation->store.key = malloc(key_size);
	if (!CHECK_RECV_WITH_SIZE(esi.fd, operation->store.key, key_size)) {
		esi_log_error(logger, "Error al recibir la clave!");
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

static int esi_get_id(void)
{
	static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
	static int current_id = 1;

	int id;
	synchronized(lock) {
		id = current_id;
		current_id++;
	}

	return id;
}

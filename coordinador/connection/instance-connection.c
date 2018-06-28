#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../../protocolo/protocolo_coordinador_instancia.h"

#include "../defines.h"
#include "../logger.h"
#include "../config.h"
#include "../distribution.h"
#include "../connection/esi-connection.h"
#include "../connection/scheduler-connection.h"
#include "../instance-list/instance-list.h"
#include "../instance-list/instance-request-list.h"

t_log *logger;
struct setup_t setup;

struct instance_list_t *instance_list;

static void instance_handle_disconnection(struct instance_t *instance);
static bool instance_handle_set_request(struct instance_t *instance, struct request_node_t *request);
static bool instance_handle_store_request(struct instance_t *instance, struct request_node_t *request);
static char *instance_recv_name(int fd);
static bool instance_recv_set_execution_status(int fd, int *status_code, size_t *used_entries);
static bool instance_recv_store_execution_status(int fd, int *status_code);
static bool instance_send_confirmation(int fd);
static bool instance_send_confirmation_error(int fd);
static bool instance_send_set_instruction(int fd, char *key, char *value);
static bool instance_send_store_instruction(int fd, char *key);

__attribute__((constructor)) void init_instance_list(void) {
	instance_list = instance_list_create();
}

__attribute__((destructor)) void destroy_instance_list(void) {
	instance_list_destroy(instance_list);
}

static char *instance_recv_name(int fd);

/* TODO:
 *   - Reasignar Instancia si se desconecta cuando se esta procesando una operacion.
 */

void handle_instance_connection(int fd)
{
	char *name = instance_recv_name(fd);
	if (name == NULL) {
		instance_send_confirmation_error(fd);
		return;
	}

	struct instance_t *instance;
	synchronized(instance_list->lock) {
		instance = instance_list_add(instance_list, name, fd);
	}
	free(name);

	if (instance == NULL) {
		instance_send_confirmation_error(fd);
		log_error(logger, "[Instancia] Socket %d: Ya existe otra instancia con nombre \"%s\"!", fd, instance->name);
		return;
	}

	log_info(logger, "[Instancia] Socket %d: Instancia \"%s\" conectada.", fd, instance->name);

	if (!instance_send_confirmation(fd)) {
		log_error(logger, "[Instancia %s] Error al enviar confirmacion!", instance->name);
		return;
	}

	bool error = false;
	while (!error) {
		struct request_node_t *request = request_list_pop(instance->requests);
		log_info(logger, "[Instancia %s] Atendiendo pedido...", instance->name);

		switch(request->type) {
		case INSTANCE_SET:
			error = !instance_handle_set_request(instance, request);
			break;
		case INSTANCE_STORE:
			error = !instance_handle_store_request(instance, request);
			break;
		}

		if (!error) {
			request_node_destroy(request);
		} else {
			request_list_push(instance->requests, request);
			instance_handle_disconnection(instance);
		}
	}
}

static void instance_handle_disconnection(struct instance_t *instance)
{
	synchronized(instance_list->lock) {
		instance_list_remove(instance_list, instance->name);
	}

	void reselect_instance(void *_elem) {
		struct request_node_t *request = (struct request_node_t *)_elem;
		char *key = request->type == INSTANCE_SET ? request->set.key : request->store.key;

		struct instance_t *instance = dispatch(instance_list, key);
		if (instance != NULL) {
			request_list_push(instance->requests, request);
		} else {
			log_error(logger, "No hay Instancias disponibles!");
			esi_send_illegal_operation(request->requesting_esi_fd);
		}
	}

	request_list_iterate(instance->requests, reselect_instance);
}

static bool instance_handle_set_request(struct instance_t *instance, struct request_node_t *request)
{
	if (!instance_send_set_instruction(instance->fd, request->set.key, request->set.value)) {
		log_error(logger, "[Instancia %s] Error al enviar instruccion SET!", instance->name);
		log_error(logger, "[Instancia %s] Se bloqueara el ESI en ejecucion!", instance->name);
		esi_send_notify_block(request->requesting_esi_fd);

		return false;
	}

	int status;
	size_t used_entries;
	if (!instance_recv_set_execution_status(instance->fd, &status, &used_entries)) {
		log_error(logger, "[Instancia %s] Error al recibir resultado de ejecucion!", instance->name);
		log_error(logger, "[Instancia %s] Se bloqueara el ESI en ejecucion!", instance->name);
		esi_send_notify_block(request->requesting_esi_fd);

		return false;
	}

	instance->used_entries += used_entries;

	/* status:
	 *   (1) success.
	 *   (0) failure.
	 */
	if (status == 1) {
		log_info(logger, "[Instancia %s] Operacion realizada correctamente.", instance->name);
		esi_send_execution_success(request->requesting_esi_fd);
		return true;
	} else if (status == 0) {
		/* TODO: REVIEW */
		log_error(logger, "[Instancia %s] Operacion fallida!", instance->name);
		esi_send_illegal_operation(request->requesting_esi_fd);
		return false;
	} else {
		log_error(logger, "[Instancia %s] Se recibio un resultado invalido!", instance->name);
		esi_send_notify_block(request->requesting_esi_fd);
		return false;
	}
}

static bool instance_handle_store_request(struct instance_t *instance, struct request_node_t *request)
{
	if (!instance_send_store_instruction(instance->fd, request->store.key)) {
		log_error(logger, "[Instancia %s] Error al enviar instruccion STORE!", instance->name);
		log_error(logger, "[Instancia %s] Se bloqueara el ESI en ejecucion!", instance->name);
		esi_send_notify_block(request->requesting_esi_fd);

		return false;
	}

	int status;
	if (!instance_recv_store_execution_status(instance->fd, &status)) {
		log_error(logger, "[Instancia %s] Error al recibir resultado de ejecucion!", instance->name);
		log_error(logger, "[Instancia %s] Se bloqueara el ESI en ejecucion!", instance->name);
		esi_send_notify_block(request->requesting_esi_fd);

		return false;
	}

	/* status:
	 *   (1) success.
	 *   (0) failure.
	 */
	if (status == 1) {
		log_info(logger, "[Instancia %s] Operacion realizada correctamente.", instance->name);
		/* TODO: Manejar desconexion del planificador. */
		scheduler_unblock_key(request->store.key);
		esi_send_execution_success(request->requesting_esi_fd);
		return true;
	} else if (status == 0) {
		/* TODO: REVIEW */
		log_error(logger, "[Instancia %s] Operacion fallida!", instance->name);
		esi_send_illegal_operation(request->requesting_esi_fd);
		return false;
	} else {
		log_error(logger, "[Instancia %s] Se recibio un resultado invalido!", instance->name);
		esi_send_notify_block(request->requesting_esi_fd);
		return false;
	}
}

static char *instance_recv_name(int fd)
{
	size_t size;
	if (!CHECK_RECV(fd, &size)) {
		log_error(logger, "[Instancia] Socket %d: Error al recibir tamanio del nombre de instancia!");
		return NULL;
	}

	char *name = malloc(size);
	if (!CHECK_RECV_WITH_SIZE(fd, name, size)) {
		log_error(logger, "[Instancia] Socket %d: Error al recibir el nombre de instancia!");
		free(name);
		return NULL;
	}

	return name;
}

static bool instance_recv_set_execution_status(int fd, int *status_code, size_t *used_entries)
{
	request_instancia op_code;
	if (!CHECK_RECV(fd, &op_code) || op_code != PROTOCOL_IC_NOTIFY_STATUS) {
		return false;
	}

	if (!CHECK_RECV(fd, status_code)) {
		return false;
	}

	return CHECK_RECV(fd, used_entries);
}

static bool instance_recv_store_execution_status(int fd, int *status_code)
{
	request_instancia op_code;
	if (!CHECK_RECV(fd, &op_code) || op_code != PROTOCOL_IC_NOTIFY_STORE) {
		return false;
	}
	return CHECK_RECV(fd, status_code);
}

static bool instance_send_confirmation(int fd)
{
	struct {
		size_t entry_size;
		size_t entry_num;
	} PACKED package;

	package.entry_size = setup.entries_size;
	package.entry_num = setup.entries_num;

	return CHECK_SEND(fd, &package);
}

static bool instance_send_confirmation_error(int fd)
{
	struct {
		request_coordinador op_code;
		bool validation;
	} PACKED package;

	package.op_code = PROTOCOL_CI_HANDSHAKE_CONFIRMATION;
	package.validation = false;

	return CHECK_SEND(fd, &package);
}

static bool instance_send_set_instruction(int fd, char *key, char *value)
{
	request_coordinador op_code = PROTOCOL_CI_SET;
	size_t key_size = strlen(key) + 1;
	size_t value_size = strlen(value) + 1;

	struct { void *block; size_t block_size; } blocks[] = {
		{ &op_code, sizeof(op_code) },
		{ &key_size, sizeof(key_size) },
		{ key, key_size },
		{ &value_size, sizeof(value_size) },
		{ value, value_size }
	};

	int i;
	for (i = 0; i < sizeof(blocks) / sizeof(*blocks); i++) {
		if (!CHECK_SEND_WITH_SIZE(fd, blocks[i].block, blocks[i].block_size)) {
			return false;
		}
	}

	return true;
}

static bool instance_send_store_instruction(int fd, char *key)
{
	request_coordinador op_code = PROTOCOL_CI_STORE;
	size_t key_size = strlen(key) + 1;

	struct { void *block; size_t block_size; } blocks[] = {
		{ &op_code, sizeof(op_code) },
		{ &key_size, sizeof(key_size) },
		{ key, key_size },
	};

	int i;
	for (i = 0; i < sizeof(blocks) / sizeof(*blocks); i++) {
		if (!CHECK_SEND_WITH_SIZE(fd, blocks[i].block, blocks[i].block_size)) {
			return false;
		}
	}

	return true;
}

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
#include "../key-table/key-table.h"

t_log *logger;
struct setup_t setup;

struct instance_list_t *instance_list;

static void instance_handle_disconnection(struct instance_t *instance);
static bool instance_handle_set_request(struct instance_t *instance, struct request_node_t *request);
static bool instance_handle_store_request(struct instance_t *instance, struct request_node_t *request);
static char *instance_recv_name(int fd);
static bool instance_recv_set_execution_status(int fd, int *status_code, size_t *used_entries);
static bool instance_recv_store_execution_status(int fd, int *status_code);
static bool instance_send_confirmation(struct instance_t *instance);
static bool instance_send_confirmation_error(int fd);
static bool instance_send_set_instruction(int fd, char *key, char *value);
static bool instance_send_store_instruction(int fd, char *key);
static void instance_send_compact(struct instance_t *requested_instance);

__attribute__((constructor)) void init_instance_list(void) {
	instance_list = instance_list_create();
}

__attribute__((destructor)) void destroy_instance_list(void) {
	instance_list_destroy(instance_list);
}

static char *instance_recv_name(int fd);

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

	if (instance == NULL) {
		instance_send_confirmation_error(fd);
		log_error(logger, "[Instancia] Socket %d: Ya existe otra instancia con nombre \"%s\"!", fd, name);
		free(name);
		return;
	}
	free(name);

	log_info(logger, "[Instancia] Socket %d: Instancia \"%s\" conectada.", fd, instance->name);

	if (!instance_send_confirmation(instance)) {
		log_error(logger, "[Instancia %s] Error al enviar confirmacion!", instance->name);
		return;
	}

	bool error = false;
	while (!error) {
		struct request_node_t *request = request_list_pop(instance->requests);
		log_info(logger, "[Instancia %s] Atendiendo pedido...", instance->name);

		switch (request->type) {
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
			/* TODO: Definir un protocolo distinto para informar este tipo de error. */
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

	instance->used_entries = used_entries;

	enum set_status_t {
		SET_STATUS_COMPACT = 2,
		SET_STATUS_OK = 1,
		SET_STATUS_REPLACED = 0,
		SET_STATUS_NO_SPACE = -1
	};

	switch (status) {
	case SET_STATUS_OK:
		log_info(logger, "[Instancia %s] Operacion realizada correctamente.", instance->name);
		key_table_set_initialized(request->set.key);
		esi_send_execution_success(request->requesting_esi_fd);
		return true;
	case SET_STATUS_COMPACT:
		log_info(logger, "[Instancia %s] Se necesita hacer una compactacion.", instance->name);
		instance_send_compact(instance);
		log_info(logger, "[Instancia %s] Operacion realizada correctamente.", instance->name);
		key_table_set_initialized(request->set.key);
		esi_send_execution_success(request->requesting_esi_fd);
		return true;
	case SET_STATUS_REPLACED:
		log_error(logger, "[Instancia %s] La clave fue previamente reemplazada!", instance->name);
		esi_send_illegal_operation(request->requesting_esi_fd);
		return true;
	case SET_STATUS_NO_SPACE:
		log_error(logger, "[Instancia %s] No hay espacio disponible!", instance->name);
		esi_send_illegal_operation(request->requesting_esi_fd);
		return true;
	default:
		log_error(logger, "[Instancia %s] Error de comunicacion!", instance->name);
		esi_send_illegal_operation(request->requesting_esi_fd);
		return false;
	}
}

static bool instance_handle_store_request(struct instance_t *instance, struct request_node_t *request)
{
	if (key_table_is_new(request->store.key)) {
		log_error(logger, "[Instancia %s] Error al realizar un STORE sobre una clave no inicializada.", instance->name);
		esi_send_illegal_operation(request->requesting_esi_fd);
		return true;
	}

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

	enum store_status_t {
		STORE_STATUS_OK = 1,
		STORE_STATUS_REPLACED = 0,
	};

	switch (status) {
	case STORE_STATUS_OK:
		log_info(logger, "[Instancia %s] Operacion realizada correctamente.", instance->name);
		/* TODO: Manejar desconexion del planificador. */
		scheduler_unblock_key(request->store.key);
		esi_send_execution_success(request->requesting_esi_fd);
		return true;
	case STORE_STATUS_REPLACED:
		log_error(logger, "[Instancia %s] La clave fue previamente reemplazada!", instance->name);
		esi_send_illegal_operation(request->requesting_esi_fd);
		return true;
	default:
		log_error(logger, "[Instancia %s] Error de comunicacion!", instance->name);
		esi_send_illegal_operation(request->requesting_esi_fd);
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

static bool instance_send_confirmation(struct instance_t *instance)
{
	struct {
		size_t entry_size;
		size_t entry_num;
		size_t key_list_size;
	} PACKED header;

	header.entry_size = setup.entries_size;
	header.entry_num = setup.entries_num;

	char **key_list = key_table_get_instance_key_list(instance, &header.key_list_size);

	if (!CHECK_SEND(instance->fd, &header)) {
		free(key_list);
		return false;
	}

	if (header.key_list_size > 0) {
		log_info(logger, "[Instancia] Socket %d: Enviando lista de claves a recuperar (%d claves)...",
				instance->fd, header.key_list_size);
	}

	int i;
	for (i = 0; i < header.key_list_size; i++) {
		size_t key_size = strlen(key_list[i]) + 1;
		if (!CHECK_SEND(instance->fd, &key_size)) {
			free(key_list);
			return false;
		} else if (!CHECK_SEND_WITH_SIZE(instance->fd, key_list[i], key_size)) {
			free(key_list);
			return false;
		}
	}

	return true;
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
	bool is_key = key_table_is_new(key);
	size_t key_size = strlen(key) + 1;
	size_t value_size = strlen(value) + 1;

	struct { void *block; size_t block_size; } blocks[] = {
		{ &op_code, sizeof(op_code) },
		{ &is_key, sizeof(is_key) },
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
		{ key, key_size }
	};

	int i;
	for (i = 0; i < sizeof(blocks) / sizeof(*blocks); i++) {
		if (!CHECK_SEND_WITH_SIZE(fd, blocks[i].block, blocks[i].block_size)) {
			return false;
		}
	}

	return true;
}

static void instance_send_compact(struct instance_t *requested_instance)
{
	request_coordinador op_code = PROTOCOL_CI_COMPACT;
	void send_compact(void *elem) {
		struct instance_t *instance = (struct instance_t *)elem;
		if (instance != requested_instance && CHECK_SEND(instance->fd, &op_code)) {
			log_info(logger, "[Instancia] Socket %d: Pedido de compactacion enviada.", instance->fd);
		}
	}

	instance_list_iterate(instance_list, send_compact);
}

bool instance_request_value(int fd, char *key, char **value)
{
	request_coordinador request_op_code = PROTOCOL_CI_REQUEST_VALUE;
	size_t key_size = strlen(key) + 1;

	struct { void *block; size_t block_size; } blocks[] = {
		{ &request_op_code, sizeof(request_op_code) },
		{ &key_size, sizeof(key_size) },
		{ key, key_size }
	};

	int i;
	for (i = 0; i < sizeof(blocks) / sizeof(*blocks); i++) {
		if (!CHECK_SEND_WITH_SIZE(fd, blocks[i].block, blocks[i].block_size)) {
			return false;
		}
	}

	request_instancia response_op_code;
	if (!CHECK_RECV(fd, &response_op_code) || response_op_code != PROTOCOL_IC_RETRIEVE_VALUE) {
		return false;
	}

	enum value_status_t {
		VALUE_STATUS_OK = 1, VALUE_STATUS_REPLACED = 0
	};

	int status;
	if (!CHECK_RECV(fd, &status)) {
		return false;
	}
	switch (status) {
	case VALUE_STATUS_OK: ;
		size_t value_size;
		if (!CHECK_RECV(fd, &value_size) || value_size <= 0) {
			return false;
		}

		*value = malloc(value_size);
		if (!CHECK_RECV_WITH_SIZE(fd, *value, value_size)) {
			return false;
		} else {
			return true;
		}
	case VALUE_STATUS_REPLACED:
		*value = NULL;
		return true;
	default:
		return false;
	}
}

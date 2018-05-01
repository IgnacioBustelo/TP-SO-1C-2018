#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "../logger.h"
#include "../config.h"
#include "../instance-list/instance-list.h"
#include "../instance-list/instance-request-list.h"

#define CHECK_SEND_WITH_SIZE(FD, BLOCK, BLOCK_SIZE) (send(FD, BLOCK, BLOCK_SIZE, 0) != (BLOCK_SIZE))
#define CHECK_SEND(FD, BLOCK) CHECK_SEND_WITH_SIZE(FD, BLOCK, sizeof(*(BLOCK)))

#define CHECK_RECV_WITH_SIZE(FD, BLOCK, BLOCK_SIZE) (recv(FD, BLOCK, BLOCK_SIZE, MSG_WAITALL) != (BLOCK_SIZE))
#define CHECK_RECV(FD, BLOCK) CHECK_RECV_WITH_SIZE(FD, BLOCK, sizeof(*(BLOCK)))

#define PACKED __attribute__((packed))

/* Esto debe estar en otro lugar. */
enum protocol_t {
	PROTOCOL_CI_HANDSHAKE_CONFIRMATION,
	PROTOCOL_CI_SET,
	PROTOCOL_IC_NOTIFY_STATUS
};

t_log *logger;
struct setup_t setup;

struct instance_list_t *instance_list;

static char *instance_recv_name(int fd);
static bool instance_recv_execution_status(int fd, int *status_code);
static bool instance_send_confirmation(int fd);
static bool instance_send_confirmation_error(int fd);
static bool instance_send_set_instruction(int fd, char *key, char *value);

__attribute__((constructor)) void init_instance_list(void) {
	instance_list = instance_list_create();
}

__attribute__((destructor)) void destroy_instance_list(void) {
	/* TODO */
}

static char *instance_recv_name(int fd);

void handle_instance_connection(int fd)
{
	char *name = instance_recv_name(fd);
	if (name == NULL) {
		/* Informar error a la instancia? */
		return;
	}
	log_info(logger, "[Instancia] Socket %d: Nombre de instancia: \"%s\"", name);

	if (!instance_send_confirmation(fd)) {
		log_error(logger, "[Instancia %s] Error al enviar confirmacion!", name);
	}

	struct instance_t *instance = instance_list_add(instance_list, name, fd);

	for (;;) {
		struct request_node_t *request = request_list_pop(instance->requests);
		log_info(logger, "[Instancia %s] Atendiendo pedido...", name);

		bool success = false;
		switch (request->type) {
		case INSTANCE_SET:
			success = instance_send_set_instruction(fd, request->set.key, request->set.value);
			break;
		case INSTANCE_STORE:
			/* TODO:
			 * success = instance_send_store_instruction(fd, request->store.key);
			 */
			break;
		}
	}

	instance_list_remove(instance_list, name);
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

static bool instance_recv_execution_status(int fd, int *status_code)
{
	return CHECK_RECV(fd, status_code);
}

static bool instance_send_confirmation(int fd)
{
	struct {
		enum protocol_t op_code;
		bool validation;
		size_t entry_size;
		size_t entry_num;
	} PACKED package;

	package.op_code = PROTOCOL_CI_HANDSHAKE_CONFIRMATION;
	package.validation = true;
	package.entry_size = setup.entries_size;
	package.entry_num = setup.entries_num;

	return CHECK_SEND(fd, &package);
}

static bool instance_send_confirmation_error(int fd)
{
	struct {
		enum protocol_t op_code;
		bool validation;
	} PACKED package;

	package.op_code = PROTOCOL_CI_HANDSHAKE_CONFIRMATION;
	package.validation = false;

	return CHECK_SEND(fd, &package);
}

static bool instance_send_set_instruction(int fd, char *key, char *value)
{
	enum protocol_t op_code = PROTOCOL_CI_SET;
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

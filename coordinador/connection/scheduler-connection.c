#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <unistd.h>
#include <commons/collections/queue.h>

#include "../logger.h"
#include "../defines.h"
#include "../instance-list/instance-list.h"
#include "../key-table/key-table.h"
#include "../../protocolo/protocolo.h"

#include "instance-connection.h"
#include "scheduler-connection.h"

t_log *logger;

static bool scheduler_connected;
static int scheduler_fd;

sem_t pending_protocol_sem;
protocol_id pending_protocol;

sem_t protocol_recv_sem;

static bool scheduler_send_key_status(void);
static void scheduler_pending_protocol_set(protocol_id protocol);
static protocol_id scheduler_pending_protocol_recv();
static void scheduler_exit(void);

__attribute__((destructor)) void close_scheduler_fd(void) {
	if (scheduler_connected) {
		sem_destroy(&pending_protocol_sem);
		close(scheduler_fd);
	}
}

void handle_scheduler_connection(int fd)
{
	scheduler_connected = true;
	scheduler_fd = fd;

	sem_init(&pending_protocol_sem, 0, 0);
	sem_init(&protocol_recv_sem, 0, 1);

	for ( ; ; ) {
		sem_wait(&protocol_recv_sem);
		protocol_id op_code;
		if (!CHECK_RECV(fd, &op_code)) {
			scheduler_exit();
		}

		switch (op_code) {
		case PROTOCOL_PC_KEY_STATUS:
			if (!scheduler_send_key_status()) {
				scheduler_exit();
			}
			sem_post(&protocol_recv_sem);
			break;
		default:
			scheduler_pending_protocol_set(op_code);
			break;
		}
	}
}

enum key_state_t scheduler_recv_key_state(char *key)
{
	protocol_id op_code = PROTOCOL_CP_IS_THIS_KEY_BLOCKED;
	size_t key_size = strlen(key) + 1;

	struct { void *block; size_t block_size; } blocks[] = {
		{ &op_code, sizeof(op_code) },
		{ &key_size, sizeof(key_size) },
		{ key, key_size },
	};

	int i;
	for (i = 0; i < sizeof(blocks) / sizeof(*blocks); i++) {
		if (!CHECK_SEND_WITH_SIZE(scheduler_fd, blocks[i].block, blocks[i].block_size)) {
			scheduler_exit();
		}
	}

	enum key_state_t key_state;

	protocol_id blocked_state_response = scheduler_pending_protocol_recv();
	if (blocked_state_response == PROTOCOL_PC_KEY_IS_NOT_BLOCKED) {
		key_state = KEY_UNBLOCKED;
	} else if (blocked_state_response == PROTOCOL_PC_KEY_IS_BLOCKED) {
		int blocked_by_executing_esi_op_code = PROTOCOL_CP_IS_KEY_BLOCKED_BY_EXECUTING_ESI;
		if (!CHECK_SEND(scheduler_fd, &blocked_by_executing_esi_op_code)) {
			scheduler_exit();
		}

		protocol_id blocked_by_executing_esi_response;

		if (!CHECK_RECV(scheduler_fd, &blocked_by_executing_esi_response)) {
			scheduler_exit();
		} else if (blocked_by_executing_esi_response == PROTOCOL_PC_KEY_BLOCKED_BY_EXECUTING_ESI) {
			key_state = KEY_BLOCKED_BY_EXECUTING_ESI;
		} else if (blocked_by_executing_esi_response == PROTOCOL_PC_KEY_NOT_BLOCKED_BY_EXECUTING_ESI) {
			key_state = KEY_BLOCKED;
		} else {
			scheduler_exit();
		}
	} else {
		scheduler_exit();
	}

	sem_post(&protocol_recv_sem);
	return key_state;
}

static bool scheduler_send_key_status(void)
{
	size_t key_size;
	if (!CHECK_RECV(scheduler_fd, &key_size)) {
		return false;
	}

	char *key = malloc(key_size);
	if (!CHECK_RECV_WITH_SIZE(scheduler_fd, key, key_size)) {
		free(key);
		return false;
	}

	protocol_id op_code = PROTOCOL_CP_KEY_STATUS;
	if (!CHECK_SEND(scheduler_fd, &op_code)) {
		free(key);
		return false;
	}

	enum key_state_t {
		KEY_INSTANCE_DISCONNECTED = -2,
		KEY_NOT_EXIST = -1,
		KEY_UNINITIALIZED = 0,
		KEY_INITIALIZED = 1
	};

	bool send_key_state(int state) {
		return CHECK_SEND(scheduler_fd, &state);
	}

	bool send_key_instance(char *instance_name) {
		size_t instance_name_size = strlen(instance_name) + 1;
		if (!CHECK_SEND(scheduler_fd, &instance_name_size)) {
			return false;
		} else if (!CHECK_SEND_WITH_SIZE(scheduler_fd, instance_name, instance_name_size)) {
			return false;
		} else {
			return true;
		}
	}

	bool send_key_value(char *value) {
		size_t value_size = strlen(value) + 1;
		if (!CHECK_SEND(scheduler_fd, &value_size)) {
			return false;
		} else if (!CHECK_SEND_WITH_SIZE(scheduler_fd, value, value_size)) {
			return false;
		} else {
			return true;
		}
	}

	struct instance_t *instance = key_table_get_instance(key);
	if (instance == NULL) {
		free(key);
		return send_key_state(KEY_NOT_EXIST);
	} else if (key_table_is_new(key)) {
		free(key);
		return send_key_state(KEY_UNINITIALIZED)
				&& send_key_instance(instance->name);
	} else {
		char *value;
		if (instance->fd != DISCONNECTED && instance_request_value(instance->fd, key, &value)) {
			free(key);
			return send_key_state(KEY_INITIALIZED)
					&& send_key_instance(instance->name)
					&& send_key_value(value);
		} else {
			free(key);
			return send_key_state(KEY_NOT_EXIST);
			/* TODO: Se necesita un mensaje KEY_INSTANCE_DISCONNECTED. */
			//		return send_key_state(KEY_INSTANCE_DISCONNECTED)
			//				&& send_key_instance(instance->name);
		}
	}
}

bool scheduler_block_key(void)
{
	protocol_id request = PROTOCOL_CP_BLOCK_KEY;
	if (!CHECK_SEND(scheduler_fd, &request)) {
		return false;
	}

	protocol_id response = scheduler_pending_protocol_recv();
	sem_post(&protocol_recv_sem);
	return response == PROTOCOL_PC_KEY_BLOCKED_SUCCESFULLY;
}

bool scheduler_unblock_key(char *key)
{
	protocol_id request = PROTOCOL_CP_UNLOCK_KEY;
	size_t key_size = strlen(key) + 1;

	struct { void *block; size_t block_size; } blocks[] = {
		{ &request, sizeof(request) },
		{ &key_size, sizeof(key_size) },
		{ key, key_size },
	};

	int i;
	for (i = 0; i < sizeof(blocks) / sizeof(*blocks); i++) {
		if (!CHECK_SEND_WITH_SIZE(scheduler_fd, blocks[i].block, blocks[i].block_size)) {
			return false;
		}
	}

	protocol_id response = scheduler_pending_protocol_recv();
	sem_post(&protocol_recv_sem);
	return response == PROTOCOL_PC_KEY_UNLOCKED_SUCCESFULLY;
}

static void scheduler_pending_protocol_set(protocol_id protocol)
{
	pending_protocol = protocol;
	sem_post(&pending_protocol_sem);
}

static protocol_id scheduler_pending_protocol_recv()
{
	sem_wait(&pending_protocol_sem);
	protocol_id protocol = pending_protocol;
	return protocol;
}

static void scheduler_exit(void)
{
	log_error(logger, "Finalizando el programa por la desconexion del Planificador...");
	exit(EXIT_FAILURE);
}

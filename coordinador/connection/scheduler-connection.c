#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../defines.h"
#include "../key-table/key-table.h"
#include "../../protocolo/protocolo.h"

#include "scheduler-connection.h"

static bool scheduler_connected;
static int scheduler_fd;

static bool scheduler_send_key_status(void);
static void *scheduler_recv(size_t size);
static void scheduler_unrecv(void *data, size_t size);

__attribute__((destructor)) void close_scheduler_fd(void) {
	if (scheduler_connected) {
		close(scheduler_fd);
	}
}

/* TODO:
 *   - Synchronization
 */

static struct {
	void *data;
	size_t size;
} recv_buffer = { NULL, 0 };

void handle_scheduler_connection(int fd)
{
	scheduler_connected = true;
	scheduler_fd = fd;

	for ( ; ; ) {
		protocol_id op_code;
		if (!CHECK_RECV(fd, &op_code)) {
			/* TODO: Manejar desconexion de todos los hilos. */
			exit(EXIT_FAILURE);
		}

		switch (op_code) {
		case PROTOCOL_PC_KEY_STATUS:
			/* TODO: Manejar desconexion de todos los hilos. */
			if (!scheduler_send_key_status()) {
				exit(EXIT_FAILURE);
			}
			break;
		default:
			scheduler_unrecv(&op_code, sizeof(op_code));
			break;
		}
	}
}

enum key_state_t scheduler_recv_key_state(char *key)
{
	int op_code = PROTOCOL_CP_IS_THIS_KEY_BLOCKED;
	size_t key_size = strlen(key) + 1;

	struct { void *block; size_t block_size; } blocks[] = {
		{ &op_code, sizeof(op_code) },
		{ &key_size, sizeof(key_size) },
		{ key, key_size },
	};

	int i;
	for (i = 0; i < sizeof(blocks) / sizeof(*blocks); i++) {
		if (!CHECK_SEND_WITH_SIZE(scheduler_fd, blocks[i].block, blocks[i].block_size)) {
			return KEY_RECV_ERROR;
		}
	}

	protocol_id blocked_state_response;
	if (!CHECK_RECV(scheduler_fd, &blocked_state_response)) {
		return KEY_RECV_ERROR;
	}
	if (blocked_state_response == PROTOCOL_PC_KEY_IS_NOT_BLOCKED) {
		return KEY_UNBLOCKED;
	} else if (blocked_state_response == PROTOCOL_PC_KEY_IS_BLOCKED) {
		protocol_id blocked_by_executing_esi_response;

		if (!CHECK_RECV(scheduler_fd, &blocked_by_executing_esi_response)) {
			return KEY_RECV_ERROR;
		} else if (blocked_by_executing_esi_response == PROTOCOL_PC_KEY_BLOCKED_BY_EXECUTING_ESI) {
			return KEY_BLOCKED_BY_EXECUTING_ESI;
		} else if (blocked_by_executing_esi_response == PROTOCOL_PC_KEY_NOT_BLOCKED_BY_EXECUTING_ESI) {
			return KEY_BLOCKED;
		} else {
			return KEY_RECV_ERROR;
		}
	} else {
		return KEY_RECV_ERROR;
	}
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

	enum key_state { KEY_NOT_EXIST, KEY_UNINITIALIZED, KEY_INITIALIZED };

	char *instance_name = key_table_get_instance_name(key);
	if (instance_name == NULL) {
		free(key);
		int key_state = KEY_NOT_EXIST;

		return CHECK_SEND(scheduler_fd, &key_state);
	}

	/* TODO: Pedir el valor a la instancia. */
	int key_state = KEY_UNINITIALIZED;
	if (!CHECK_SEND(scheduler_fd, &key_state)) {
		free(key);
		return false;
	}

	size_t instance_name_size = strlen(instance_name) + 1;
	if (!CHECK_SEND(scheduler_fd, &instance_name_size)) {
		free(key);
		return false;
	}
	if (!CHECK_SEND_WITH_SIZE(scheduler_fd, instance_name, instance_name_size)) {
		free(key);
		return false;
	}

	return true;
}

bool scheduler_block_key(void)
{
	protocol_id request = PROTOCOL_CP_BLOCK_KEY;
	if (!CHECK_SEND(scheduler_fd, &request)) {
		return false;
	}

	protocol_id response;
	if (!CHECK_RECV(scheduler_fd, &response)) {
		return false;
	} else {
		return response == PROTOCOL_PC_KEY_BLOCKED_SUCCESFULLY;
	}
}

bool scheduler_unblock_key(void)
{
	protocol_id request = PROTOCOL_CP_UNLOCK_KEY;
	if (!CHECK_SEND(scheduler_fd, &request)) {
		return false;
	}

	protocol_id response;
	if (!CHECK_RECV(scheduler_fd, &response)) {
		return false;
	} else {
		return response == PROTOCOL_PC_KEY_UNLOCKED_SUCCESFULLY;
	}
}

static void *scheduler_recv(size_t size)
{
	if (recv_buffer.size <= size) {
		void *ret = recv_buffer.data;
		ret = realloc(ret, size);
		if (recv_buffer.size != size &&
			!CHECK_RECV_WITH_SIZE(scheduler_fd, &ret[recv_buffer.size], size - recv_buffer.size))
		{
			free(ret);
			recv_buffer.data = NULL;
			recv_buffer.size = 0;
			return NULL;
		} else {
			return ret;
		}
	} else {
		void *ret = recv_buffer.data;
		recv_buffer.data = malloc(recv_buffer.size - size);
		memcpy(recv_buffer.data, &ret[size], recv_buffer.size - size);
		recv_buffer.size -= size;
		ret = realloc(ret, size);

		return ret;
	}
}

static void scheduler_unrecv(void *data, size_t size)
{
	void *new_data = malloc(size + recv_buffer.size);
	memcpy(new_data, data, size);

	if (recv_buffer.size > 0) {
		memcpy(&new_data[size], recv_buffer.data, recv_buffer.size);
	}

	free(recv_buffer.data);
	recv_buffer.data = new_data;
	recv_buffer.size += size;
}

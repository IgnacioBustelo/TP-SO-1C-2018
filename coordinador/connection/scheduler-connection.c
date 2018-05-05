#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>

#include "../defines.h"
#include "../../protocolo/protocolo.h"

#include "scheduler-connection.h"

static int scheduler_fd;

void handle_scheduler_connection(int fd)
{
	scheduler_fd = fd;
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

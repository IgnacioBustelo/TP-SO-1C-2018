#ifndef COORDINADOR_CONNECTION_SCHEDULER_CONNECTION_H_
#define COORDINADOR_CONNECTION_SCHEDULER_CONNECTION_H_

enum key_state_t {
	KEY_RECV_ERROR = -1,
	KEY_UNBLOCKED, KEY_BLOCKED, KEY_BLOCKED_BY_EXECUTING_ESI
};

void handle_scheduler_connection(int fd);
enum key_state_t scheduler_recv_key_state(char *key);

#endif /* COORDINADOR_CONNECTION_SCHEDULER_CONNECTION_H_ */

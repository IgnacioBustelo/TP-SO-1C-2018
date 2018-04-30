#ifndef COORDINADOR_CONNECTION_ESI_CONNECTION_H_
#define COORDINADOR_CONNECTION_ESI_CONNECTION_H_

#define CHECK_RECV_WITH_SIZE(FD, BLOCK, BLOCK_SIZE) (recv(FD, BLOCK, BLOCK_SIZE, MSG_WAITALL) != (BLOCK_SIZE))
#define CHECK_RECV(FD, BLOCK) CHECK_RECV_WITH_SIZE(FD, BLOCK, sizeof(*(BLOCK)))

enum operation_t {
	ESI_GET, ESI_SET, ESI_STORE
};

struct esi_operation_t {
	enum operation_t keyword;
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

void handle_esi_connection(int fd);

#endif /* COORDINADOR_CONNECTION_ESI_CONNECTION_H_ */

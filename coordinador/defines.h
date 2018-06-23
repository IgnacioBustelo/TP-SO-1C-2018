#ifndef COORDINADOR_DEFINES_H_
#define COORDINADOR_DEFINES_H_

#define synchronized(lock)											\
	for (pthread_mutex_t * i_lock = &lock; i_lock;					\
		i_lock = NULL, pthread_mutex_unlock(&lock))					\
		for (pthread_mutex_lock(i_lock); i_lock; i_lock = NULL)

#define CHECK_SEND_WITH_SIZE(FD, BLOCK, BLOCK_SIZE) (send(FD, BLOCK, BLOCK_SIZE, MSG_NOSIGNAL) == (BLOCK_SIZE))
#define CHECK_SEND(FD, BLOCK) CHECK_SEND_WITH_SIZE(FD, BLOCK, sizeof(*(BLOCK)))

#define CHECK_RECV_WITH_SIZE(FD, BLOCK, BLOCK_SIZE) (recv(FD, BLOCK, BLOCK_SIZE, MSG_WAITALL) == (BLOCK_SIZE))
#define CHECK_RECV(FD, BLOCK) CHECK_RECV_WITH_SIZE(FD, BLOCK, sizeof(*(BLOCK)))

#define PACKED __attribute__((packed))

#endif /* COORDINADOR_DEFINES_H_ */

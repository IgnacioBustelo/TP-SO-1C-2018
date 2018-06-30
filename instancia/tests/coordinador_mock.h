#ifndef INSTANCIA_TESTS_COORDINADOR_MOCK_H_
#define INSTANCIA_TESTS_COORDINADOR_MOCK_H_

#include <commons/collections/list.h>
#include <stdbool.h>
#include <stdlib.h>

void	coordinador_mock_handshake(int fd_client, bool is_accepted, size_t total_entries, size_t entry_size, t_list* recoverable_keys);

void	coordinador_mock_set_request(int fd_client, bool is_new, char* key, char* value);

int		coordinador_mock_set_response(int fd_client);

void	coordinador_mock_store_request(int fd_client, char* key);

int		coordinador_mock_store_response(int fd_client);

void	coordinador_mock_status_request(int fd_client, char* key);

char*	coordinador_mock_status_response(int fd_client);

void	coordinador_mock_kill(int fd_client);

#endif

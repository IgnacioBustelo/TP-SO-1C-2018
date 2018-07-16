#ifndef INSTANCIA_TESTS_COORDINADOR_MOCK_H_
#define INSTANCIA_TESTS_COORDINADOR_MOCK_H_

#include <commons/collections/list.h>
#include <stdbool.h>
#include <stdlib.h>

void	coordinador_mock_init();

void	coordinador_mock_handshake(int fd_client, int total_entries, int entry_size, t_list* recoverable_keys);

void	coordinador_mock_set(int fd_client, char* key, char* value);

void	coordinador_mock_store(int fd_client, char* key);

void	coordinador_mock_status(int fd_client, char* key);

void	coordinador_mock_handshake_base(int fd_client, bool* is_accepted);

void	coordinador_mock_handshake_receive_name(int fd_client, char** received_name);

void	coordinador_mock_handshake_send_config(int fd_client, char* received_name, size_t total_entries, size_t entry_size, t_list* recoverable_keys);

void	coordinador_mock_set_request(int fd_client, bool is_new, char* key, char* value);

int		coordinador_mock_set_response(int fd_client);

void	coordinador_mock_store_request(int fd_client, char* key);

int		coordinador_mock_store_response(int fd_client);

void	coordinador_mock_status_request(int fd_client, char* key);

char*	coordinador_mock_status_response(int fd_client, char* key);

void	coordinador_mock_check_request(int fd_client);

int		coordinador_mock_check_response(int fd_client);

void	coordinador_mock_kill(int fd_client);

void	coordinador_mock_destroy();

#endif

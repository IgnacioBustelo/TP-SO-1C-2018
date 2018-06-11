#ifndef INSTANCIA_TESTS_COORDINADOR_MOCK_H_
#define INSTANCIA_TESTS_COORDINADOR_MOCK_H_

void coordinador_mock_handshake(int fd_client, size_t total_entries, size_t entry_size);

void coordinador_mock_set_request(int fd_client, char* key, char* value);

int	 coordinador_mock_set_response(int fd_client);

void coordinador_mock_store_request(int fd_client, char* key);

int	 coordinador_mock_store_response(int fd_client);

void coordinador_mock_kill(int fd_client);

#endif

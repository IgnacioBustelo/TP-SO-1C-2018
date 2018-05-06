#ifndef COORDINADOR_CONNECTION_ESI_CONNECTION_H_
#define COORDINADOR_CONNECTION_ESI_CONNECTION_H_

void handle_esi_connection(int fd);
bool esi_send_execution_success(int fd);
bool esi_send_notify_block(int fd);
bool esi_send_illegal_operation(int fd);

#endif /* COORDINADOR_CONNECTION_ESI_CONNECTION_H_ */

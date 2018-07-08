#ifndef PROTOCOLO_PROTOCOLO_COORDINADOR_INSTANCIA_H_
#define PROTOCOLO_PROTOCOLO_COORDINADOR_INSTANCIA_H_

#define C_HEADER(X)		protocolo_coordinador_instancia_coordinador_header_to_string(X)
#define I_HEADER(X)		protocolo_coordinador_instancia_instancia_header_to_string(X)
#define CI_STATUS(X)	protocolo_coordinador_instancia_status_to_string(X)

typedef enum {
	PROTOCOL_CI_HANDSHAKE_CONFIRMATION,
	PROTOCOL_CI_SET,
	PROTOCOL_CI_STORE,
	PROTOCOL_CI_COMPACT,
	PROTOCOL_CI_REQUEST_VALUE,
	PROTOCOL_CI_IS_ALIVE,
	PROTOCOL_CI_KILL
} request_coordinador;

typedef enum {
	PROTOCOL_IC_HANDSHAKE,
	PROTOCOL_IC_NOTIFY_STATUS,
	PROTOCOL_IC_NOTIFY_STORE,
	PROTOCOL_IC_RETRIEVE_VALUE,
	PROTOCOL_IC_CONFIRM_CONNECTION,
	PROTOCOL_IC_DIE
} request_instancia;

typedef enum {
	STATUS_COMPACT = 2,
	STATUS_OK = 1,
	STATUS_REPLACED = 0,
	STATUS_NO_SPACE = -1,
	STATUS_ERROR = -2
} status_ci;

const char* protocolo_coordinador_instancia_coordinador_header_to_string(request_coordinador header);

const char* protocolo_coordinador_instancia_instancia_header_to_string(request_instancia header);

const char* protocolo_coordinador_instancia_status_to_string(status_ci status);

#endif

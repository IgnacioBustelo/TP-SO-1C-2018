#include "protocolo_coordinador_instancia.h"

const char* protocolo_coordinador_instancia_coordinador_header_to_string(request_coordinador header) {
	switch(header) {
		case PROTOCOL_CI_HANDSHAKE_CONFIRMATION:	return "PROTOCOL_CI_HANDSHAKE_CONFIRMATION";	break;

		case PROTOCOL_CI_SET:						return "PROTOCOL_CI_SET";						break;

		case PROTOCOL_CI_STORE:						return "PROTOCOL_CI_STORE";						break;

		case PROTOCOL_CI_COMPACT:					return "PROTOCOL_CI_COMPACT";					break;

		case PROTOCOL_CI_REQUEST_VALUE:				return "PROTOCOL_CI_REQUEST_VALUE";				break;

		case PROTOCOL_CI_KILL:						return "PROTOCOL_CI_KILL";						break;

		default:									return "PROTOCOL_CI_UNKNOWN";					break;
	}
}

const char* protocolo_coordinador_instancia_instancia_header_to_string(request_instancia header) {
	switch(header) {
		case PROTOCOL_IC_HANDSHAKE:					return "PROTOCOL_IC_HANDSHAKE";			break;

		case PROTOCOL_IC_NOTIFY_STATUS:				return "PROTOCOL_IC_NOTIFY_STATUS";		break;

		case PROTOCOL_IC_NOTIFY_STORE:				return "PROTOCOL_IC_NOTIFY_STORE";		break;

		case PROTOCOL_IC_RETRIEVE_VALUE:			return "PROTOCOL_IC_RETRIEVE_VALUE";	break;

		case PROTOCOL_IC_DIE:						return "PROTOCOL_IC_DIE";				break;

		default:									return "PROTOCOL_IC_UNKNOWN";			break;
	}
}

const char* protocolo_coordinador_instancia_status_to_string(status_ci status) {
	switch(status) {
		case STATUS_COMPACT:						return "STATUS_COMPACT";	break;

		case STATUS_OK:								return "STATUS_OK";			break;

		case STATUS_REPLACED:						return "STATUS_REPLACED";	break;

		case STATUS_NO_SPACE:						return "STATUS_NO_SPACE";	break;

		default:									return "STATUS_UNKNOWN";	break;
	}
}

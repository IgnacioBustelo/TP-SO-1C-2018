#include "instancia.h"
#include "coordinator_api.h"

int main(void) {
	coordinator_api_handshake();

	for(;;) {
		request_coordinador header;
		int status;

		recv_package(fd_coordinador, &header, sizeof(request_coordinador));

		switch(header) {
			case PROTOCOL_CI_SET: {
				key_value_t* key_value = coordinator_api_receive_set();

				instance_set(key_value);

				key_value_destroy(key_value);

				status = 1;

				break;
			}

			default: {
				status = 0;

				break;
			}
		}

		coordinator_api_notify_status(status);
	}
}

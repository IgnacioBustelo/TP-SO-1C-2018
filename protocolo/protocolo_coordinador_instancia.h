#ifndef PROTOCOLO_PROTOCOLO_COORDINADOR_INSTANCIA_H_
#define PROTOCOLO_PROTOCOLO_COORDINADOR_INSTANCIA_H_

typedef enum {
	PROTOCOL_CI_HANDSHAKE_CONFIRMATION,
	PROTOCOL_CI_SET,
	PROTOCOL_CI_STORE,
	PROTOCOL_CI_KILL
} request_coordinador;

typedef enum {
	PROTOCOL_IC_HANDSHAKE,
	PROTOCOL_IC_NOTIFY_STATUS,
	PROTOCOL_IC_NOTIFY_STORE,
	PROTOCOL_IC_DIE
} request_instancia;

/* TODO: Agregar siguiente lista mensajes
 *
 * Discutir si algunas necesitan respuesta o no (COMPACT y RECONNECT no deberian)
 *
 * case PROTOCOL_CI_ROLLBACK => Devuelve la lista de claves que haya que reincorporar, aunque se puede mandar en el Handshake
 *
 * case PROTOCOL_CI_COMPACT => Envia la orden de compactar a la Instancia
 *
 * case PROTOCOL_CI_STATUS_REQUEST => Pide el contenido de una/muchas claves
 *
 * case PROTOCOL_IC_STATUS_REPORT => Devuelve el contenido de una/muchas claves
*/

#endif

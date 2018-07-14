#include <pthread.h>
#include <unistd.h>

#include "../libs/configurator.h"
#include "../libs/messenger.h"

#include "algorithms.h"
#include "cfg_instancia.h"
#include "compactation.h"
#include "coordinator_api.h"
#include "dumper.h"
#include "entry_table.h"
#include "event_handler.h"
#include "instancia.h"
#include "storage.h"

#define IP					cfg_instancia_get_coordinador_ip()
#define HOST				cfg_instancia_get_coordinador_port()
#define NAME				cfg_instancia_get_instance_name()
#define MOUNT_POINT			cfg_instancia_get_mount_point()
#define DUMP_INTERVAL		cfg_instancia_get_dump_time()
#define ALGORITHM_ID		cfg_instancia_get_replacement_algorithm_id()

#define R_CHECK(ERR_CODE, MSG, SET_CODE)	\
	if(operation_result == ERR_CODE) {		\
		messenger_show("ERROR", MSG);		\
											\
		return SET_CODE;					\
	}										\

#define B_CHECK(ERR_CODE, MSG, SET_CODE)	\
	if(operation_result == ERR_CODE) {		\
		messenger_show("ERROR", MSG);		\
											\
		request_result = SET_CODE;			\
											\
		break;								\
	}										\

#define API_R_CHECK(MSG)	R_CHECK(API_ERROR, MSG, INSTANCE_API_ERROR)
#define API_B_CHECK(MSG)	B_CHECK(API_ERROR, MSG, INSTANCE_API_ERROR)

pthread_mutex_t instance_mutex = PTHREAD_MUTEX_INITIALIZER;

void _dump(void* key) {
	entry_t* entry = entry_table_get_entry((char*) key);

	void* data = storage_retrieve(entry->number, entry->size);

	dumper_store(key, data, entry->size);

	free(data);
}

int instance_init(char* process_name, char* logger_route, char* log_level, char* cfg_route) {
	int status;

	storage_setup_t	setup;
	t_list*			recoverable_keys;

	event_handler_init_with_finisher(instance_die);

	messenger_init(logger_route, process_name, log_level);

	cfg_instancia_init(cfg_route);

	status = instance_handshake(&setup, &recoverable_keys);

	if(status != INSTANCE_HANDSHAKE_SUCCESS) {
		switch(status) {
			case INSTANCE_HANDSHAKE_REJECTED: {
				messenger_show("WARNING", "El Coordinador rechazo a la Instancia dado que existe otra con el mismo nombre");

				break;
			}

			default: {
				messenger_show("ERROR", "Ocurrio un error en la comunicacion para efectuar el Handshake");

				break;
			}
		}

		messenger_show("ERROR", "No se pudo conectar al Coordinador");

		configurator_destroy();

		messenger_destroy();

		return INSTANCE_INIT_ERROR;
	}

	messenger_show("INFO", "Ejecucion correcta del Handshake con el Coordinador en la IP %s en el puerto %d", IP, HOST);

	storage_setup_init(setup.total_entries, setup.entry_size);

	storage_init(setup.total_entries, setup.entry_size);

	messenger_show("INFO", "Inicio del Storage con una capacidad de %d entradas de tamano %d", setup.total_entries, setup.entry_size);

	dumper_init(MOUNT_POINT);

	messenger_show("INFO", "Inicio del Punto de Montaje a disco en el punto de montaje %s", MOUNT_POINT);

	entry_table_init();

	entry_table_status_init();

	messenger_show("INFO", "Inicio de la Tabla De Entradas");

	algorithm_circular_set_pointer(0);
	
	instance_recover(recoverable_keys);

	messenger_show("INFO", "La Instancia se inicio correctamente");

	messenger_show("INFO", "Estado inicial de la Instancia");

	instance_show();

	list_destroy_and_destroy_elements(recoverable_keys, free);

	return INSTANCE_INIT_SUCCESS;
}

int	instance_handshake(storage_setup_t* setup, t_list** recoverable_keys) {
	int operation_result;

	bool is_confirmed;

	operation_result = coordinator_api_connect(IP, HOST);
	API_R_CHECK("Fallo conectando con el Coordinador")

	operation_result = coordinator_api_handshake_base(&is_confirmed);
	API_R_CHECK("Fallo recibiendo confirmacion inicial")

	if(!is_confirmed) {
		return INSTANCE_HANDSHAKE_REJECTED;
	}

	operation_result = coordinator_api_handshake_send_name(NAME);
	API_R_CHECK("Fallo enviando nombre de la Instancia al Coordinador")

	operation_result = coordinator_api_handshake_receive_config(setup, recoverable_keys);
	API_R_CHECK("Fallo recibiendo configuracion del Coordinador")

	return INSTANCE_HANDSHAKE_SUCCESS;
}


int instance_set(key_value_t* key_value, t_list* replaced_keys) {
	int operation_result;

	if(!entry_table_has_entries(key_value) && new_value_fits(key_value) ) {
		if(!entry_table_status_continuous_entries(replaced_keys)) {
			messenger_show("INFO", "La Instancia tiene que compactar para ingresar la clave %s", key_value->key);

			operation_result = compactation_compact();
		}

		void replace_and_show_key(void* key) {
			dumper_remove_key_value((char*) key);

			messenger_show("WARNING", "La clave %s fue reemplazada por %s", (char*) key, key_value->key);
		}

		messenger_show("WARNING", "La Instancia debe ejecutar un reemplazo");

		operation_result = algorithms_exec(cfg_instancia_get_replacement_algorithm_id(), entry_table, key_value, replaced_keys);

		if(operation_result) {
			entry_table_delete_few(replaced_keys);
		}

		list_iterate(replaced_keys, (void*) replace_and_show_key);

		messenger_show("INFO", "Reemplazo ejecutado correctamente");
	}


	else if(!new_value_fits(key_value)){
		messenger_show("ERROR", "La Instancia no tiene entradas atomicas para ejecutar un reemplazo");

		return STATUS_NO_SPACE;
	}

	int next_entry = entry_table_next_entry(key_value);

	if(next_entry >= 0) {
		messenger_show("INFO", "La entrada %d esta disponible para ingresar la clave %s", next_entry, key_value->key);
	}

	else {
		messenger_show("ERROR", "No se puede ingresar la clave en la tabla de entradas");

		return STATUS_ERROR;
	}

	operation_result = storage_set(next_entry, key_value->value, key_value->size);

	if(operation_result != STRG_SUCCESS) {
		messenger_show("ERROR", "No se puede ingresar el valor de la clave %s en el Storage", key_value->key);

		return STATUS_ERROR;
	}

	messenger_show("INFO", "Se inserto el valor '%s' en el Storage", key_value->value);

	operation_result = entry_table_insert(next_entry, key_value);

	entry_table_status_add_kv(key_value,next_entry);

	if(operation_result == 1) {
		messenger_show("INFO", "Se proceso correctamente el SET de la clave %s en la entrada %d", key_value->key, next_entry);
	}

	return STATUS_OK;
}


int	instance_store(char* key) {
	messenger_show("INFO", "Se recibio un pedido de STORE de la clave %s", key);

	int status = 1;

	entry_t* entry = entry_table_get_entry(key);

	if(entry == NULL) {
		messenger_show("WARNING", "No se encontro la clave '%s' en la Tabla De Entradas", key);

		return STATUS_REPLACED;
	}

	void* data = storage_retrieve(entry->number, entry->size);

	dumper_store(key, data, entry->size); // TODO: Hacer que devuelva estado.

	if(status == 1) {
		messenger_show("INFO", "Se proceso correctamente el STORE de la clave %s", key);
	}

	else {
		messenger_show("ERROR", "Ocurrio un error ejecutando el STORE de la clave %s", key);
	}

	free(data);

	return status;
}

int instance_status(char* key, key_value_t** key_value) {
	entry_t* entry = entry_table_get_entry(key);

	if(entry == NULL) {
		 return STATUS_REPLACED;
	}

	char* value = storage_retrieve_string(entry->number, entry->size);

	*key_value = key_value_create(key, value);

	free(value);

	return STATUS_OK;
}


int	instance_recover(t_list* recoverable_keys) {
	messenger_show("INFO", "Inicio de la Recuperacion de la Instancia");

	if(list_is_empty(recoverable_keys)) {
		messenger_show("INFO", "No es necesario recuperar claves");

		return STATUS_OK;
	}

	// TODO: Manejar errores
	t_list* replaced_keys = list_create();

	void recovered_key_value_set(void* key_value) {
		instance_set((key_value_t*) key_value, replaced_keys);
	}

	messenger_show("INFO", "Es necesario recuperar %d claves", list_size(recoverable_keys));

	t_list* recovered_key_values = dumper_recover(recoverable_keys);

	if(list_size(recovered_key_values) < list_size(recoverable_keys)) {
		messenger_show("WARNING", "Solo se pueden recuperar %d clave/s de %d pedida/s", list_size(recovered_key_values), list_size(recoverable_keys));
	}

	else {
		messenger_show("INFO", "Se puden recuperar todas las claves solicitadas");
	}

	list_iterate(recovered_key_values, (void*) recovered_key_value_set);

	t_list* recovered_keys = entry_table_get_key_list();
	
	char* recovered_keys_csv = messenger_list_to_string(recovered_keys);

	messenger_show("INFO", "Se recuperaron las claves [%s]", recovered_keys_csv);

	free(recovered_keys_csv);

	list_destroy(recovered_keys);

	list_destroy_and_destroy_elements(replaced_keys, (void*) key_value_destroy);

	list_destroy_and_destroy_elements(recovered_key_values, (void*) key_value_destroy);

	messenger_show("INFO", "Fin de la recuperacion de la Instancia");

	return STATUS_OK;
}


void instance_thread_api(void* args) {
	int status, operation_result, request_result;

	messenger_show("INFO", "Comienzo de actividades de la Instancia");

	while(instance_is_alive) {
		messenger_show("INFO", "Esperando peticion del Coordinador");

		request_coordinador header;

		operation_result = coordinator_api_receive_header(&header);

		if(operation_result == API_ERROR) {
			messenger_show("ERROR", "Fallo en la recepcion del pedido del Coordinador");

			instance_is_alive = false;

			break;
		}

		API_B_CHECK("Fallo en la recepcion del pedido del Coordinador")

		switch(header) {
			case PROTOCOL_CI_SET: {
				key_value_t*	key_value;
				bool			is_new;

				operation_result = coordinator_api_receive_set(&is_new, &key_value);
				API_B_CHECK("Fallo en la recepcion del pedido del SET del Coordinador")

				t_list* replaced_keys = list_create();

				pthread_mutex_lock(&instance_mutex);

				if(!(is_new || entry_table_has_key(key_value->key, is_new))) {
					messenger_show("WARNING", "La clave solicitada no existe en la Instancia dado que fue reemplazada");

					key_value_destroy(key_value);

					coordinator_api_notify_set(STATUS_REPLACED, get_total_entries() - entries_left);

					request_result = INSTANCE_REQUEST_FAILURE;

					break;
				}

				status = instance_set(key_value, replaced_keys);

				pthread_mutex_unlock(&instance_mutex);

				operation_result = coordinator_api_notify_set(get_total_entries() - entries_left, status);
				API_B_CHECK("Fallo en el envio del resultado del SET al Coordinador")

				request_result = INSTANCE_REQUEST_SUCCESS;

				key_value_destroy(key_value);

				list_destroy_and_destroy_elements(replaced_keys, free);

				break;
			}

			case PROTOCOL_CI_STORE: {
				char* key;

				operation_result = coordinator_api_receive_key(&key);
				API_B_CHECK("Fallo en la recepcion del pedido del STORE del Coordinador")

				pthread_mutex_lock(&instance_mutex);

				status = instance_store(key);

				pthread_mutex_unlock(&instance_mutex);

				operation_result = coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, status);
				API_B_CHECK("Fallo en el envio del resultado del STORE al Coordinador")

				request_result = INSTANCE_REQUEST_SUCCESS;

				free(key);

				break;
			}

			case PROTOCOL_CI_COMPACT: {
				messenger_show("INFO", "Pedido de compactacion");

				pthread_mutex_lock(&instance_mutex);

				operation_result = compactation_compact();

				pthread_mutex_unlock(&instance_mutex);

				messenger_show("INFO", "La Instancia se compacto");

				request_result = INSTANCE_REQUEST_SUCCESS;

				break;
			}

			case PROTOCOL_CI_REQUEST_VALUE: {
				char*			requested_key;
				key_value_t*	requested_key_value;

				operation_result = coordinator_api_receive_key(&requested_key);
				API_B_CHECK("Fallo en la recepcion del pedido del valor de una clave del Coordinador")

				messenger_show("INFO", "Pedido del valor de la clave %s", requested_key);

				pthread_mutex_lock(&instance_mutex);

				status = instance_status(requested_key, &requested_key_value);

				pthread_mutex_unlock(&instance_mutex);

				if(status == STATUS_REPLACED) {
					messenger_show("WARNING", "No se encontro la clave %s en la Tabla De Entradas", requested_key);

					operation_result = coordinator_api_notify_status(PROTOCOL_IC_RETRIEVE_VALUE, status);
					API_B_CHECK("Fallo en el envio del estado de la clave")
				}

				else {
					messenger_show("INFO", "Envio del valor '%s' de la clave '%s'", requested_key_value->value, requested_key_value->key);

					operation_result = coordinator_api_notify_key_value(requested_key_value, status);
					API_B_CHECK("Fallo en el envio del valor de una clave al Coordinador")

					key_value_destroy(requested_key_value);
				}

				free(requested_key);

				request_result = INSTANCE_REQUEST_SUCCESS;

				break;
			}

			case PROTOCOL_CI_IS_ALIVE: {
				messenger_show("INFO", "La Instancia recibio un pedido del Coordinador para chequear si sigue conectada");

				coordinator_api_notify_header(PROTOCOL_IC_CONFIRM_CONNECTION);
				API_B_CHECK("Fallo en el envio del estado del estado de la conexion")

				request_result = INSTANCE_REQUEST_SUCCESS;

				break;
			}

			case PROTOCOL_CI_KILL: {
				messenger_show("INFO", "La Instancia recibio un pedido del Coordinador para desconectarse");

				instance_is_alive = false;

				break;
			}

			default: {
				messenger_show("INFO", "Se recibio un mensaje no esperado");

				request_result = INSTANCE_API_ERROR;

				break;
			}
		}

		switch(request_result) {
			case INSTANCE_REQUEST_SUCCESS: {
				messenger_show("INFO", "Procesamiento correcto del pedido %s del Coordinador", C_HEADER(header));

				break;
			}

			case INSTANCE_REQUEST_FAILURE: {
				messenger_show("ERROR", "Error procesando el pedido %s del Coordinador", C_HEADER(header));

				instance_is_alive = false;

				break;
			}

			case INSTANCE_COMPACT: {
				messenger_show("WARNING", "La Instancia requiere compactarse");

				pthread_mutex_lock(&instance_mutex);

				compactation_compact();

				pthread_mutex_unlock(&instance_mutex);

				messenger_show("INFO", "Fin de la compactacion de la Instancia");

				break;
			}

			case INSTANCE_API_ERROR: {
				messenger_show("INFO", "Hubo un problema en la conexion durante el procesamiento del pedido");

				instance_is_alive = false;

				break;
			}

			case INSTANCE_DIE: {
				messenger_show("INFO", "La Instancia se va a desconectar por pedido del Coordinador");

				instance_is_alive = false;

				break;
			}

			default: {
				messenger_show("INFO", "Se recibio el mensaje '%d' no identificado y se va desconectar la Instancia", request_result);

				instance_is_alive = false;

				break;
			}
		}
	}
}

void instance_thread_dump(void* args) {
	int time_passed = 0;

	if(DUMP_INTERVAL == 0) {
		messenger_show("WARNING", "Decidimos no ejecutar el Dump cuando el intervalo del mismo es 0");

		pthread_exit(NULL);
	}

	while(instance_is_alive) {
		sleep(DUMP_INTERVAL);

		time_passed += DUMP_INTERVAL;

		pthread_mutex_lock(&instance_mutex);

		messenger_show("DEBUG", "Ejecutando Dump en tras %d ms", time_passed);

		t_list* stored_keys = entry_table_get_key_list();

		if(!list_is_empty(stored_keys)) {
			list_iterate(stored_keys, (void*) _dump);

			char* stored_keys_csv = messenger_list_to_string(stored_keys);

			messenger_show("DEBUG", "Se Dumpeo el valor de las claves [%s]", stored_keys_csv);

			free(stored_keys_csv);
		}

		else {
			messenger_show("DEBUG", "No hay claves para Dumpear");
		}

		list_destroy_and_destroy_elements(stored_keys, free);

		messenger_show("DEBUG", "Fin de ejecucion de Dump");

		pthread_mutex_unlock(&instance_mutex);
	}
}

void instance_main() {
	pthread_t api_thread, dump_thread;

	pthread_create(&api_thread, NULL, (void*) instance_thread_api, NULL);
	pthread_create(&dump_thread, NULL, (void*) instance_thread_dump, NULL);

	pthread_join(api_thread, NULL);
	pthread_join(dump_thread, NULL);
}

void instance_show() {
	messenger_show("INFO", "Parametros del archivo de configuracion");

	configurator_read();

	messenger_show("INFO", "Estado de la Tabla de Entradas");

	entry_table_show();

	messenger_show("INFO", "Estado del Storage");

	storage_show();

	messenger_show("INFO", "Estado de las claves persistidas en disco");

	dumper_show();
}

void instance_die() {
	pthread_mutex_lock(&instance_mutex);

	messenger_show("INFO", "Fin de actividades de la Instancia");

	messenger_show("INFO", "Estado final de la Instancia");

	instance_show();

	storage_setup_destroy();

	storage_destroy();

	dumper_destroy();

	entry_table_destroy();

	entry_table_status_destroy();

	coordinator_api_disconnect();

	configurator_destroy();

	messenger_show("INFO", "Volvere y sere millone$");

	messenger_destroy();

	pthread_mutex_unlock(&instance_mutex);
}

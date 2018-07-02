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

pthread_mutex_t instance_mutex = PTHREAD_MUTEX_INITIALIZER;

int instance_init(char* process_name, char* logger_route, char* log_level, char* cfg_route) {
	storage_setup_t dimensions;
	t_list* recoverable_keys;

	event_handler_init();

	messenger_init(logger_route, process_name, log_level);

	cfg_instancia_init(cfg_route);

	configurator_read();

	coordinator_api_connect(IP, HOST);

	int status = coordinator_api_handshake(NAME, &dimensions, &recoverable_keys);

	if(status == API_HANDSHAKE_ERROR) {
		coordinator_api_disconnect();

		configurator_destroy();

		messenger_destroy();

		return INSTANCE_INIT_ERROR;
	}

	storage_setup_init(dimensions.total_entries, dimensions.entry_size);

	storage_init(dimensions.total_entries, dimensions.entry_size);

	dumper_init(MOUNT_POINT);

	entry_table_init();

	algorithm_circular_set_pointer(0);

	instance_recover(recoverable_keys);

	event_handler_alarm(DUMP_INTERVAL);

	messenger_show("INFO", "La Instancia se inicio correctamente");

	messenger_show("INFO", "Estado inicial de la Instancia");

	instance_show();

	list_destroy_and_destroy_elements(recoverable_keys, free);

	return INSTANCE_INIT_SUCCESS;
}

int instance_set(key_value_t* key_value, t_list* replaced_keys) {
	int status = INSTANCE_SUCCESS;

	// TODO: Chequear tambien que haya suficiente entradas atomicas para reemplazar

	if(!entry_table_have_entries(key_value) /* && entry_table_has_atomic_values() */) {
		void replace_and_show_key(void* key) {
			dumper_remove_key_value((char*) key);

			messenger_show("WARNING", "La clave %s fue reemplazada por %s", (char*) key, key_value->key);
		}

		messenger_show("WARNING", "La Instancia debe ejecutar un reemplazo");

		/* TODO: Reemplazar por algo mas polimorfico:
		 *
		 * int status = algorithms_exec(id, entry_table, key_value, replaced_keys);
		 *
		 * if(status == TIE && status > 0) {
		 * 		// Ejecutar el algoritmo circular en caso de empate entre claves a reemplazar
		 *
		 * 		status = algorithms_exec('C', entry_table, key_value, replaced_keys);
		 * }
		 *
		 * entry_table_delete_few(replaced_keys);
		 *
		 */

		algorithm_circular(entry_table, key_value, replaced_keys); // TODO: Cuando se implemente lo de arriba, reemplazar

		entry_table_delete_few(replaced_keys);

		list_iterate(replaced_keys, (void*) replace_and_show_key);

		messenger_show("INFO", "Reemplazo ejecutado correctamente");
	}

	/*

	else {
		messenger_show("ERROR", "La Instancia no tiene entradas atomicas para ejecutar un reemplazo");

		return 0;
	}

	if(TODO: bool entry_table_requires_compact(size) -> Dado el tamaño de una clave, decidir si requiere compactacion la Instancia) {
		messenger_show("INFO", "La Instancia tiene que compactar para ingresar la clave %s", key_value->key);

		status = instance_compact();
	}

	*/

	int next_entry = entry_table_next_entry(key_value);

	if(next_entry >= 0) {
		messenger_show("INFO", "La entrada %d esta disponible para ingresar la clave %s", next_entry, key_value->key);
	}

	else {
		status = INSTANCE_SET_ERROR;

		messenger_show("ERROR", "No se puede ingresar la clave en la tabla de entradas");

		return status;
	}

	status = storage_set(next_entry, key_value->value, key_value->size);

	if(status != STRG_SUCCESS) {
		status = INSTANCE_SET_ERROR;

		messenger_show("ERROR", "No se puede ingresar el valor de la clave %s en el Storage", key_value->key);

		return status;
	}

	messenger_show("INFO", "Se inserto el valor '%s' en el Storage", key_value->value);

	status = entry_table_insert(next_entry, key_value);

	if(status == 1) {
		messenger_show("INFO", "Se proceso correctamente el SET de la clave %s en la entrada %d", key_value->key, next_entry);
	}

	return status;
}

int	instance_store(char* key) {
	messenger_show("INFO", "Se recibio un pedido de STORE de la clave %s", key);

	int status = 1;

	entry_t* entry = entry_table_get_entry(key);

	if(entry == NULL) {
		messenger_show("WARNING", "No se encontro la clave '%s' en la Tabla De Entradas", key);

		return INSTANCE_STORE_ERROR;
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

	return STATUS_OK;
}

int	instance_dump(t_list* stored_keys) {
	messenger_show("INFO", "Inicio de dump en la Instancia");

	int status = 1;

	void dump(void* key) {
		entry_t* entry = entry_table_get_entry((char*) key);

		void* data = storage_retrieve(entry->number, entry->size);

		dumper_store(key, data, entry->size);
	}

	list_iterate(stored_keys, (void*) dump);

	messenger_show("INFO", "Fin de dump en la Instancia");

	event_handler_alarm(DUMP_INTERVAL);

	return status;
}

int	instance_recover(t_list* recoverable_keys) {
	int status = INSTANCE_RECOVER_SUCCESS;
	t_list* replaced_keys = list_create();

	void recovered_key_value_set(void* key_value) {
		status = instance_set((key_value_t*) key_value, replaced_keys);
	}

	messenger_show("INFO", "Inicio de la Recuperacion de la Instancia");

	if(list_is_empty(recoverable_keys)) {
		messenger_show("INFO", "No es necesario recuperar claves");

		return status;
	}

	messenger_show("INFO", "Es necesario recuperar %d claves", list_size(recoverable_keys));

	t_list *recovered_keys = dumper_recover(recoverable_keys);

	if(list_size(recovered_keys) < list_size(recoverable_keys)) {
		messenger_show("WARNING", "Solo se pudieron recuperar %d claves de %d pedidas", list_size(recovered_keys), list_size(recoverable_keys));
	}

	else {
		messenger_show("INFO", "Se pudieron recuperar todas las claves solicitadas");
	}

	list_iterate(recovered_keys, (void*) recovered_key_value_set);

	list_destroy_and_destroy_elements(recovered_keys, (void*) key_value_destroy);

	messenger_show("INFO", "Fin de la recuperacion de la Instancia");

	return status;
}

void instance_thread_api(void* args) {
	messenger_show("INFO", "Comienzo de actividades de la Instancia");

	while(instance_is_alive) {

		messenger_show("INFO", "Esperando peticion del Coordinador");

		int status;

		switch(coordinator_api_receive_header()) {
			case PROTOCOL_CI_SET: {
				bool is_new;

				key_value_t* key_value = coordinator_api_receive_set(&is_new);

				/* TODO: Crear funcion entry_table_has_key(key_value->key), que dada una clave, determina si existe

				if(!(is_new || entry_table_has_key(key_value->key))) {
					messenger_show("WARNING", "La clave solicitada no existe en la Instancia dado que fue reemplazada");

					key_value_destroy(key_value);

					coordinator_api_notify_set(STATUS_REPLACED, get_total_entries() - entries_left);

					break;
				}

				*/

				t_list* replaced_keys = list_create();

				pthread_mutex_lock(&instance_mutex);

				status = instance_set(key_value, replaced_keys);

				pthread_mutex_unlock(&instance_mutex);

				key_value_destroy(key_value);

				list_destroy_and_destroy_elements(replaced_keys, free);

				coordinator_api_notify_set(status, get_total_entries() - entries_left);

				break;
			}

			case PROTOCOL_CI_STORE: {
				char* key = coordinator_api_receive_key();

				pthread_mutex_lock(&instance_mutex);

				status = instance_store(key);

				pthread_mutex_unlock(&instance_mutex);

				coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, status);

				break;
			}

			case PROTOCOL_CI_COMPACT: {
				messenger_show("INFO", "La Instancia recibio un pedido del Coordinador para compactarse");

				pthread_mutex_lock(&instance_mutex);

				compactation_compact();

				pthread_mutex_unlock(&instance_mutex);

				break;
			}

			case PROTOCOL_CI_REQUEST_VALUE: {
				char* requested_key = coordinator_api_receive_key();
				key_value_t* requested_key_value;

				messenger_show("INFO", "La Instancia recibio un pedido del valor de la clave %s", requested_key);

				pthread_mutex_lock(&instance_mutex);

				status = instance_status(requested_key, &requested_key_value);

				pthread_mutex_unlock(&instance_mutex);

				if(status == STATUS_REPLACED) {
					messenger_show("WARNING", "No se encontro la clave %s en la Tabla De Entradas", requested_key);

					coordinator_api_notify_status(PROTOCOL_IC_RETRIEVE_VALUE, status);
				}

				else {
					messenger_show("INFO", "Se retornara el valor '%s' de la clave '%s'", requested_key_value->value, requested_key_value->key);

					coordinator_api_notify_key_value(status, requested_key_value);

					key_value_destroy(requested_key_value);
				}

				free(requested_key);

				break;
			}

			case PROTOCOL_CI_KILL: {
				messenger_show("INFO", "La Instancia recibio un pedido del Coordinador para desconectarse");

				instance_is_alive = false;

				break;
			}

			default: {
				messenger_show("INFO", "Se recibio un mensaje no esperado");

				instance_is_alive = false;

				break;
			}
		}

		if(status == STATUS_COMPACT) {
			messenger_show("INFO", "Comienzo de compactacion de la Instancia");

			pthread_mutex_lock(&instance_mutex);

			compactation_compact();

			pthread_mutex_unlock(&instance_mutex);

			messenger_show("INFO", "Fin de la compactacion de la Instancia");
		}

		/*

		TODO: Hacer la funcion t_list* entry_table_get_keys(); -> Obtiene lista de claves de la tabla de entradas

		TODO: Hacer en un hilo aparte

		if(instance_requires_dump) {
			t_list* stored_keys = entry_table_get_keys();

			int status = instance_dump(stored_keys);

			if(status == -1) {
				instance_is_alive = false;
			}
		}

		*/

	}
}

void instance_thread_dump(void* args) {
	for(;;) {
		pthread_mutex_lock(&instance_mutex);

		usleep(DUMP_INTERVAL);

		messenger_show("INFO", "Ejecutando Dump");

		messenger_show("INFO", "Fin de ejecucion de Dump");

		pthread_mutex_unlock(&instance_mutex);
	}
}

void instance_main() {
	pthread_t api_thread, dump_thread;
	pthread_attr_t attributes;

	pthread_attr_init(&attributes);
	pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED);

	pthread_create(&api_thread, NULL, (void*) instance_thread_api, NULL);
	pthread_create(&dump_thread, &attributes, (void*) instance_thread_dump, NULL);

	pthread_join(api_thread, NULL);

	pthread_attr_destroy(&attributes);
}

void instance_show() {
	messenger_show("INFO", "Estado de la Tabla de Entradas");

	entry_table_print_table();

	messenger_show("INFO", "Estado del Storage");

	storage_show();

	messenger_show("INFO", "Estado de Claves Persistidas");

	dumper_show();
}

void instance_die() {
	messenger_show("INFO", "Fin de actividades de la Instancia");

	messenger_show("INFO", "Estado final de la Instancia");

	instance_show();

	storage_destroy();

	dumper_destroy();

	// entry_table_destroy(); TODO: Falta el destroyer de la tabla de entradas

	coordinator_api_disconnect();

	messenger_show("INFO", "Volvere y sere millone$");

	configurator_destroy();

	messenger_destroy();
}

#include "../libs/configurator.h"
#include "../libs/messenger.h"
#include "algorithms.h"
#include "cfg_instancia.h"
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

// TODO: Hacer que cada operacion devuelva un estado y decidir si retornar o no error.

void instance_init(char* process_name, char* logger_route, char* log_level, char* cfg_route) {
	storage_setup_t dimensions;

	event_handler_init();

	messenger_init(logger_route, process_name, log_level);

	cfg_instancia_init(cfg_route);

	configurator_read();

	coordinator_api_connect(IP, HOST);

	coordinator_api_handshake(NAME, &dimensions);

	// TODO: Discutir la reincorporacion

	storage_setup_init(dimensions.total_entries, dimensions.entry_size);

	storage_init(dimensions.total_entries, dimensions.entry_size);

	dumper_init(MOUNT_POINT);

	event_handler_alarm(DUMP_INTERVAL);

	entry_table_init();

	algorithm_circular_set_pointer(0);

	messenger_show("INFO", "La Instancia se inicio correctamente");
}

int instance_set(key_value_t* key_value, t_list* replaced_keys) {
	int status = INSTANCE_SUCCESS;

	// TODO: Chequear tambien que haya suficiente entradas atomicas para reemplazar
	// Nice to have => Que lo tenga entry_table_haSSSSSSS_entries

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

	int status = 1; // TODO: Hacer que cada operacion devuelva un estado y decidir si retornar o no error.

	entry_t* entry = entry_table_get_entry(key);

	void* data = storage_retrieve(entry->number, entry->size);

	dumper_store(key, data, entry->size);

	if(status == 1) {
		messenger_show("INFO", "Se proceso correctamente el STORE de la clave %s", key);
	}

	else {
		messenger_show("ERROR", "Ocurrio un error ejecutando el STORE de la clave %s", key);
	}

	free(data);

	return status;
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

/*

TODO: Implementar

1) bool entry_table_is_compacted -> Determina si el Storage esta compactado o no

2) entry_t* entry_table_get_next_to_compact() -> Devuelve la proxima entrada a compactar

3) Discutir el criterio para decidir la próxima entrada a mover.

int instance_compact() {
	messenger_show("WARNING", "Inicio de compactacion de la Instancia");

	int status = 1;

	while(!entry_table_is_compacted()) {
		entry_t* entry_to_compact = entry_table_get_next_to_compact();

		void* buffer = storage_retrieve(entry_to_compact->number, entry_to_compact->size);

		entry_table_delete(entry_to_compact->number);

		char* string_buffer = messenger_bytes_to_string(buffer, entry_to_compact->size);

		key_value_t* key_value_buffer = key_value_create(entry_to_compact->key, string_buffer);

		instance_set(key_value_buffer, NULL); -> A este paso se refiere el inciso 3.

		messenger_show("WARNING", "Se traslado la clave %s", key_value_buffer->key);

		key_value_destroy(key_value_buffer);

		free(string_buffer);

		free(buffer);

		free(entry_to_compact);
	}

	messenger_show("WARNING", "Fin de compactacion de la Instancia");

	return status;
}

*/

void instance_main() {
	messenger_show("INFO", "Comienzo de actividades de la Instancia");

	while(instance_is_alive) {

		messenger_show("INFO", "Esperando peticion del Coordinador");

		int status; // TODO: Manejar el tema de retornos de valores de operaciones.

		switch(coordinator_api_receive_header()) {

			// TODO: Acordar el tema de las claves reemplazadas y demas comunicaciones

			case PROTOCOL_CI_SET: {
				t_list* replaced_keys = list_create();

				key_value_t* key_value = coordinator_api_receive_set();

				status = instance_set(key_value, replaced_keys);

				key_value_destroy(key_value);

				if(!list_is_empty(replaced_keys)) {
					list_clean_and_destroy_elements(replaced_keys, free);
				}

				list_destroy(replaced_keys);

				coordinator_api_notify_set(status, get_total_entries() - entries_left);

				break;
			}

			case PROTOCOL_CI_STORE: {
				char* key = coordinator_api_receive_store();

				status = instance_store(key);

				coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_STORE, status);

				break;
			}

			/*
			TODO: Consultar con Fer por la compactacion. Testear compact()

			case PROTOCOL_CI_COMPACTATION: {
				status = compact();

				coordinator_api_notify_status(PROTOCOL_IC_NOTIFY_COMPACTATION, status);

				break;
			}
			*/

			case PROTOCOL_CI_KILL: {
				instance_is_alive = false;

				messenger_show("INFO", "La Instancia recibio un pedido del Coordinador para desconectarse");

				break;
			}

			default: {
				instance_is_alive = false;

				messenger_show("INFO", "Se recibio un mensaje no esperado");

				break;
			}
		}

		/*

		TODO: Hacer la funcion t_list* entry_table_get_keys(); -> Obtiene lista de claves de la tabla de entradas

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

	messenger_show("INFO", "Volvere y sere millone$");

	configurator_destroy();

	messenger_destroy();
}

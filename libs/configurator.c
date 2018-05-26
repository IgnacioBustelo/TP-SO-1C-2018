#include <commons/config.h>
#include <commons/string.h>
#include <stdlib.h>

#include "messenger.h"
#include "configurator.h"

t_list* config_field_list;

static void configurator_message_log_field_names(t_list* list) {
	char *buffer = string_duplicate("Los campos del archivo de configuracion son "), *names;

	void get_field_names(void* field) {
		char *current_field = string_duplicate((char*) field), *separator = string_duplicate(", ");

		string_append(&buffer, current_field);
		string_append(&buffer, separator);

		free(current_field);
		free(separator);
	}

	list_iterate(list, get_field_names);

	names = string_substring_until(buffer, string_length(buffer) - 2);

	messenger_show("INFO", "Los campos del archivo de configuracion son %s", names);

	free(buffer);

	free(names);
}

static void configurator_list_add_fields(t_list* list, char** fields, size_t fields_size) {
	char* current_field;

	bool field_is_not_equal(void* field) {
		return !string_equals_ignore_case(current_field, (char*) field);
	}

	int i;
	for(i = 0; i < fields_size; i++) {
		current_field = fields[i];

		if(list_all_satisfy(list, (void*) field_is_not_equal)) {
			list_add(list, (void*) string_duplicate(current_field));
		}
	}
}

static void configurator_list_message_field_value(void* field) {
	messenger_show("INFO", "El valor de %s es %s", field, config_get_string_value(config, (char*) field));
}

static void configurator_list_message_field_value_missing(void* field) {
	messenger_show("WARNING", "Falta la configuracion de %s", (char*) field);
}

static bool configurator_list_exists_field(void* field) {
	return config_has_property(config, (char*) field);
}

static bool configurator_list_not_exists_field(void* field) {
	return !config_has_property(config, (char*) field);
}

void configurator_init(char* config_path, char** fields, size_t fields_size) {
	messenger_show("INFO", "Iniciando el archivo de configruacion");

	config = config_create(config_path);

	messenger_show("INFO", "Abierto el archivo de configuracion");

	config_field_list = list_create();

	configurator_list_add_fields(config_field_list, fields, fields_size);

	configurator_message_log_field_names(config_field_list);

	messenger_show("INFO", "Finalizo la lectura inicial del archivo de configuracion");
}

void configurator_read() {
	if(list_all_satisfy(config_field_list, (void*) configurator_list_exists_field)) {
		messenger_show("INFO", "El archivo de configuracion leyo correctamente los siguientes campos con sus respectivos valores");

		list_iterate(config_field_list, (void*) configurator_list_message_field_value);
	}

	else {
		t_list* existing_fields = list_filter(config_field_list, (void*) configurator_list_exists_field);

		if(list_size(existing_fields) != 0) {
			messenger_show("INFO", "El archivo de configuracion leyo correctamente los siguientes campos con sus respectivos valores");

			list_iterate(existing_fields, (void*) configurator_list_message_field_value);
		}

		list_destroy(existing_fields);

		messenger_show("WARNING", "El archivo de configuracion no pudo encontrar los valores de los siguientes campos");

		t_list* non_existing_fields = list_filter(config_field_list, (void*) configurator_list_not_exists_field);

		list_iterate(non_existing_fields, (void*) configurator_list_message_field_value_missing);

		list_destroy(non_existing_fields);
	}
}

void configurator_destroy() {
	messenger_show("INFO", "Cerrado archivo de configuracion");

	list_destroy_and_destroy_elements(config_field_list, free);

	config_destroy(config);
}

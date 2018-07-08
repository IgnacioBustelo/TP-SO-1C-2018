#include "../mocks/color.h"
#include "../mocks/default_color.h"
#include "../mocks/printfer.h"

#include "../messenger.h"

int main(int argc, char* argv[]) {
	if(argc == 1) {
		messenger_show("ERROR", "Inserte una lista de valores para ejecutar la prueba");

		exit(EXIT_FAILURE);
	}

	t_list* string_list = list_create();

	int i;
	for(i = 1; i < argc; i++) {
		list_add(string_list, argv[i]);
	}

	char* csv = messenger_list_to_string(string_list);

	messenger_show("INFO", "La lista de strings es [%s]", csv);

	free(csv);

	list_destroy(string_list);
}

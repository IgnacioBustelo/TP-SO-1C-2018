#include <commons/collections/list.h>
#include <commons/string.h>

#include "../mocks/client_server.h"
#include "../chunker.h"
#include "../messenger.h"

typedef struct {
	int		id;
	char*	name;
	char*	surname;
	float	average;
	t_list* subjects;
} student_t;

student_t* student_send;

student_t* student_create(int id, char* name, char* surname, float average, t_list* subjects) {
	student_t* student = malloc(sizeof(student_t));

	student->id = id;
	student->name = string_duplicate(name);
	student->surname = string_duplicate(surname);
	student->average = average;
	student->subjects = list_duplicate(subjects);

	return student;
}

char* student_show(student_t* student) {
	char *subjects = string_new(), *comma = string_duplicate(", "), *student_string;

	void subjects_retriever(void* node) {
		string_append(&subjects, (char*) node);
		string_append(&subjects, comma);
	}

	list_iterate(student->subjects, (void*) subjects_retriever);

	student_string = string_from_format("alumno %s %s, de legajo %d, que cursa %sy tiene promedio %.2f", student->name, student->surname, student->id, subjects, student->average);

	free(comma);

	free(subjects);

	return student_string;
}

void student_destroy(student_t* student) {
	free(student->name);
	free(student->surname);
	list_destroy_and_destroy_elements(student->subjects, free);
	free(student);
}

void client_server_execute_server(int fd_client) {
	void packager(chunk_t* chunk, void* node) {
		chunk_add_variable(chunk, (char*) node, string_length((char*) node) + 1);
	}

	chunk_t* chunk = chunk_create();

	messenger_show("INFO", "Empaquetando el legajo");

	chunk_add(chunk, &student_send->id, sizeof(student_send->id));

	messenger_show("INFO", "Empaquetado el legajo");

	messenger_show("INFO", "Empaquetando el nombre");

	chunk_add_variable(chunk, student_send->name, string_length(student_send->name) + 1);

	messenger_show("INFO", "Empaquetado el nombre");

	messenger_show("INFO", "Empaquetando el apellido");

	chunk_add_variable(chunk, student_send->surname, string_length(student_send->name) + 1);

	messenger_show("INFO", "Empaquetado el apellido");

	messenger_show("INFO", "Empaquetando las materias");

	chunk_add_list(chunk, student_send->subjects, packager);

	messenger_show("INFO", "Empaquetadas las materias");

	messenger_show("INFO", "Empaquetando el promedio");

	chunk_add(chunk, &student_send->average, sizeof(student_send->average));

	messenger_show("INFO", "Empaquetado el promedio");

	messenger_show("INFO", "Enviando el paquete");

	student_destroy(student_send);

	chunk_send_and_destroy(fd_client, chunk);
}

void client_server_execute_client(int fd_server) {
	void* unpackager(int fd_server, int* bytes_recieved) {
		void* subject;

		*bytes_recieved = chunk_recv_variable(fd_server, &subject);

		return subject;
	}

	student_t* student_received = malloc(sizeof(student_t));

	chunk_recv(fd_server, &student_received->id, sizeof(student_received->id));

	messenger_show("INFO", "Recibiendo el paquete");

	messenger_show("INFO", "Recibido el legajo");

	messenger_show("INFO", "Recibiendo el nombre");

	chunk_recv_variable(fd_server, (void**) &student_received->name);

	messenger_show("INFO", "Recibido el nombre");

	messenger_show("INFO", "Recibiendo el apellido");

	chunk_recv_variable(fd_server, (void**) &student_received->surname);

	messenger_show("INFO", "Recibido el apellido");

	messenger_show("INFO", "Recibiendo las materias");

	chunk_recv_list(fd_server, &student_received->subjects, unpackager);

	messenger_show("INFO", "Recibidas las materias");

	messenger_show("INFO", "Recibiendo el promedio");

	chunk_recv(fd_server, &student_received->average, sizeof(student_received->average));

	messenger_show("INFO", "Recibido el promedio");

	char* student_description = student_show(student_received);

	messenger_show("INFO", "Se recibio al alumno %s", student_description);

	free(student_description);

	student_destroy(student_received);
}

int main(int argc, char* argv[]) {
	if(argc < 5) {
		messenger_show("ERROR", "Inserte un alumno con su legajo, nombre, apellido, promedio, y lista de materias que cursa");

		exit(EXIT_FAILURE);
	}

	server_name = "Serializador";
	client_name = "Deserializador";

	t_list* subject_list = list_create();

	int i;
	for(i = 5; i < argc; i++) {
		list_add(subject_list, string_duplicate(argv[i]));
	}

	student_send = student_create(atoi(argv[1]), argv[2], argv[3], atof(argv[4]), subject_list);

	list_destroy(subject_list);

	char* student_description = student_show(student_send);

	messenger_show("INFO", "Se va a empaquetar el alumno %s", student_description);

	free(student_description);

	client_server_run();
}

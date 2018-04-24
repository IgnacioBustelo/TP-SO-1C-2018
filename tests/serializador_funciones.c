#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "../libs/deserializador.h"
#include "../libs/serializador.h"
#include "../libs/conector.h"

#define HOST "127.0.0.1"
#define PORT 8080

typedef struct {
	int id;
	char* name;
	char* surname;
	float average;
} student_t;

static student_t* create_student(int id, char* name, char* surname, float average) {
	student_t* student = malloc(sizeof(student_t));

	student->id = id;
	student->name = strdup(name);
	student->surname = strdup(surname);
	student->average = average;

	return student;
}

static size_t student_size(student_t* student) {
	return sizeof(student->id) + strlen(student->name) + 1 + strlen(student->surname) + 1 + sizeof(student->average);
}

static void destroy_student(student_t* student) {
	free(student->name);
	free(student->surname);
	free(student);
}

void serialize() {
	printf("Serializador: Inicio de actividades del Serializador\n");

	student_t* student = create_student(1562526, "John", "Doemann", 7.50);
	size_t package_size = student_size(student);

	package_t* package = create_package(package_size);

	add_content(package, &student->id, sizeof(student->id));
	add_content_variable(package, student->name, strlen(student->name) + 1);
	add_content_variable(package, student->surname, strlen(student->surname) + 1);
	add_content(package, &student->average, sizeof(student->average));

	void* serialized_package = build_package(package);

	printf("Serializador: Contenido empaquetado exitosamente\nSerializador: Cediendo control al Deserializador\n");

	int deserializer_fd = connect_to_server(HOST, PORT);

	printf("Serializador: Conectado al Deserializador.\n");

	int status = send_serialized_package(deserializer_fd, serialized_package, package_size);

	if(status == SEND_ERROR) {
		printf("Serializador: Error enviando el paquete\n");
	}

	else {
		printf("Serializador: Paquete enviado exitosamente\n");
	}

	destroy_package(package);
	destroy_student(student);
	close(deserializer_fd);

	exit(EXIT_SUCCESS);
}

void deserialize() {
	printf("Deserializador: Inicio de actividades\n");

	int deserializer_fd = init_listener(PORT, 1);

	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;

	printf("Deserializador: Esperando al Serializador\nDeserializador: Cediendo control al Serializador\n");

	int serializer_fd = accept(deserializer_fd, (struct sockaddr *) &client_addr, &client_addr_size);
	printf("Deserializador: Aceptada la conexión del Serializado\nDeserializador: Esperando datos del Serializador\nDeserializador: Cediendo control al Serializador\n");

	int id;
	char* name;
	char* surname;
	float average;

	recv_package(serializer_fd, &id, sizeof(id));
	recv_package_variable(serializer_fd, (void**) &name);
	recv_package_variable(serializer_fd, (void**) &surname);
	recv_package(serializer_fd, &average, sizeof(average));

	student_t* student = create_student(id, name, surname, average);

	printf("Deserializador: Recibido al alumno %s %s, de legajo %d y promedio %f\n", student->name, student->surname, student->id, student->average);

	destroy_student(student);
	close(serializer_fd);
	close(deserializer_fd);

	exit(EXIT_SUCCESS);
}

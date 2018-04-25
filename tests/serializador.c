#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/conector.h"
#include "../libs/serializador.h"
#include "serializador.h"

student_t* student_create(int id, char* name, char* surname, float average) {
	student_t* student = malloc(sizeof(student_t));

	student->id = id;
	student->name = strdup(name);
	student->surname = strdup(surname);
	student->average = average;

	return student;
}

size_t student_size(student_t* student) {
	return sizeof(student->id) + sizeof(size_t) + strlen(student->name) + 1 + sizeof(size_t) + strlen(student->surname) + 1 + sizeof(student->average);
}

void student_destroy(student_t* student) {
	free(student->name);
	free(student->surname);
	free(student);
}

package_t* serialize() {
	student_t* student = student_create(678, "John Juan", "Doemann De Tal", 2.50);
	size_t package_size = student_size(student);

	package_t* package = create_package(package_size);

	add_content(package, &student->id, sizeof(student->id));
	add_content_variable(package, student->name, strlen(student->name) + 1);
	add_content_variable(package, student->surname, strlen(student->surname) + 1);
	add_content(package, &student->average, sizeof(student->average));

	student_destroy(student);

	return package;
}

package_t* serialize_pretty() {
	char* message;
	student_t* student = student_create(678, "John Juan", "Doemann De Tal", 2.50);
	size_t package_size = student_size(student);

	printf("Se va a empaquetar al alumno %s %s, de legajo %d y promedio %f\n", student->name, student->surname, student->id, student->average);

	package_t* package = create_package(package_size);
	package_status status;

	printf("Se debe reservar una porcion de memoria de %d bytes\n", package_size);

	status = check_package(package);
	message = status_message(package, status);
	printf("%s", message);
	free(message);

	printf("Se empaqueta el legajo del alumno de %d bytes\n", sizeof(student->id));

	status = check_package(package);
	add_content(package, &student->id, sizeof(student->id));
	message = status_message(package, status);
	printf("%s", message);
	free(message);

	printf("Se empaqueta el nombre del alumno de %d bytes mas %d bytes para el tamaño variable del nombre\n", strlen(student->name) + 1, sizeof(size_t));

	status = check_package(package);
	add_content_variable(package, student->name, strlen(student->name) + 1);
	message = status_message(package, status);
	printf("%s", message);
	free(message);

	printf("Se empaqueta el apellido del alumno de %d bytes mas %d bytes para el tamaño variable del apellido\n", strlen(student->surname) + 1, sizeof(size_t));

	status = check_package(package);
	add_content_variable(package, student->surname, strlen(student->surname) + 1);
	message = status_message(package, status);
	printf("%s", message);
	free(message);

	printf("Se empaqueta el promedio del alumno de %d bytes\n", sizeof(student->average));

	status = check_package(package);
	add_content(package, &student->average, sizeof(student->average));
	message = status_message(package, status);
	printf("%s", message);
	free(message);

	printf("Ya se puede enviar el mensaje\n");
	student_destroy(student);

	return package;
}

void send_to_deserializer(int fd, package_t* package) {
	void* serialized_package = build_package(package);

	send_serialized_package(fd, serialized_package, package->size);

	free(serialized_package);
}

void send_to_deserializer_pretty(int fd, package_t* package) {
	char* message;
	package_status status;

	status = check_package(package);
	void* serialized_package = build_package(package);
	message = status_message(package, status);
	printf("%s", message);
	free(message);

	if(serialized_package == NULL) {
		printf("Error empaquetando mensaje.\n");
		destroy_package(package);
		exit(EXIT_FAILURE);
	}

	else {
		printf("Serializacion correcta.\n");
	}

	status = send_serialized_package(fd, serialized_package, package->size);

	if(status != SEND_SUCCESS) {
		message = status_message(package, status);
		printf("%s", message);
		free(message);

		destroy_package(package);
		close(fd);
		exit(EXIT_FAILURE);
	}

	else {
		printf("Se pudo enviar el paquete serializado\n");
	}

	free(serialized_package);
}

int main(void) {
	package_t* package = serialize_pretty();

	printf("Inicio del Serializador\n");

	int fd = connect_to_server(HOST, PORT);

	printf("Conectado al Deserializador\n");

	send_to_deserializer_pretty(fd, package);

	printf("Envio del paquete al Deserializador\n");

	destroy_package(package);
	close(fd);
}


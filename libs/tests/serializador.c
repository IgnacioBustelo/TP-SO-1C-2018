#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../conector.h"
#include "../serializador.h"
#include "../serializador_v2.h"
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

void serialize_v1(int fd, student_t* student) {
	size_t package_size = student_size(student);

	package_t* package = create_package(package_size);

	add_content(package, &student->id, sizeof(student->id));
	add_content_variable(package, student->name, strlen(student->name) + 1);
	add_content_variable(package, student->surname, strlen(student->surname) + 1);
	add_content(package, &student->average, sizeof(student->average));

	void* serialized_package = build_package(package);

	send_serialized_package(fd, serialized_package, package->size);

	free(serialized_package);

	destroy_package(package);
}

package_t* serialize_v1_pretty(int fd, student_t* student) {
	char* message;
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

	status = check_package(package);
	void* serialized_package = build_package(package);
	message = status_message(package, status);
	printf("%s", message);
	free(message);

	status = send_serialized_package(fd, serialized_package, package->size);

	printf("Se pudo enviar el paquete serializado\n");

	free(serialized_package);

	destroy_package(package);

	return package;
}

void serialize_v2(int fd, student_t* student) {
	chunk_t* chunk = chunk_create();

	chunk_add(chunk, &student->id, sizeof(student->id));
	chunk_add_variable(chunk, student->name, strlen(student->name) + 1);
	chunk_add_variable(chunk, student->surname, strlen(student->name) + 1);
	chunk_add(chunk, &student->average, sizeof(student->average));

	void* serialized_chunk = chunk_build(chunk);

	send_serialized_package(fd, serialized_chunk, chunk->current_size);

	free(serialized_chunk);

	chunk_destroy(chunk);
}

int main(int argc, char* argv[]) {
	printf("Inicio del Serializador\n");

	student_t* student;

	if (argc < 5) {
		student = student_create(678, "John", "Doe", 6.50);
		printf("Como faltan parámetros, se serializa el alumno %s %s, legajo %d y promedio %f\n", student->name, student->surname, student->id, student->average);
	}

	else {
		student = student_create(atoi(argv[1]), argv[2], argv[3], atof(argv[4]));
	}

	int fd = connect_to_server(HOST, PORT);

	printf("Conectado al Deserializador\n\n");

	serialize_v1(fd, student);

	printf("Envio del paquete con la versión 1\n\n");

	serialize_v2(fd, student);

	printf("Envio del paquete con la versión 2\n");

	student_destroy(student);

	close(fd);

	exit(EXIT_SUCCESS);
}


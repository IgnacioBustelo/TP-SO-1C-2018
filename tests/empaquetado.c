#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../libs/serializador.h"
#include "empaquetado.h"

student_t* create_student(int id, char* name, char* surname, float average) {
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

void destroy_student(student_t* student) {
	free(student->name);
	free(student->surname);
	free(student);
}

void serialize() {
	printf("Inicio de serializacion\n");

	char* message;
	student_t* student = create_student(678, "John Juan", "Doemann De Tal", 2.50);
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

	if(serialized_package == NULL) {
		printf("Error empaquetando mensaje.\n");
	}

	else {
		printf("Serializacion correcta.\n");
	}

	free(serialized_package);
	destroy_package(package);
}

int main(void) {
	serialize();
}


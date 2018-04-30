#include <sys/socket.h>

#include "../conector.h"
#include "../deserializador.h"
#include "deserializador.h"

void person_destroy(person_t* student) {
	free(student->name);
	free(student->surname);
	free(student);
}

int accept_package(int deserializer_fd) {
	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;

	return accept(deserializer_fd, (struct sockaddr *) &client_addr, &client_addr_size);
}

person_t* deserialize(int serializer_fd) {
	person_t* person = malloc(sizeof(person_t));

	recv_package(serializer_fd, &person->id, sizeof(person->id));
	recv_package_variable(serializer_fd, (void**) &person->name);
	recv_package_variable(serializer_fd, (void**) &person->surname);
	recv_package(serializer_fd, &person->university_average, sizeof(person->university_average));

	return person;
}

person_t* deserialize_pretty(int serializer_fd) {
	person_t* person = malloc(sizeof(person_t));

	recv_package(serializer_fd, &person->id, sizeof(person->id));
	printf("Recibido el legajo de la persona, cuyo numero es %d\n", person->id);

	recv_package_variable(serializer_fd, (void**) &person->name);
	printf("Recibido el nombre de la persona, que se llama %s\n", person->name);

	recv_package_variable(serializer_fd, (void**) &person->surname);
	printf("Recibido el apellido de la persona, que es %s\n", person->surname);

	recv_package(serializer_fd, &person->university_average, sizeof(person->university_average));
	printf("Recibido el promedio en la universidad de la persona, que es %f\n", person->university_average);

	printf("Bienvenido al mundo %s %s, de legajo %d y promedio %f en la UTN\n", person->name, person->surname, person->id, person->university_average);

	return person;
}

int main(void) {
	printf("Inicio del Deserializador\n");

	int deserializer_fd = init_listener(PORT, 1);

	printf("Esperando al Serializador\n");

	struct sockaddr_storage client_addr;
	socklen_t client_addr_size;

	int serializer_fd = accept(deserializer_fd, (struct sockaddr *) &client_addr, &client_addr_size);

	printf("Serializador aceptado\n");

	person_t* person = deserialize_pretty(serializer_fd);

	printf("Deserializacion exitosa\n");

	person_destroy(person);
	close(serializer_fd);
	close(deserializer_fd);

	exit(EXIT_SUCCESS);
}

#ifndef TESTS_DESERIALIZADOR_H_
#define TESTS_DESERIALIZADOR_H_

#define PORT 8080

typedef struct {
	int id;
	char* name;
	char* surname;
	float university_average;
} person_t;

int accept_package(int deserializer_fd);

void person_destroy(person_t* student);

person_t* deserialize(int serializer_fd);

person_t* deserialize_pretty(int serializer_fd);

#endif

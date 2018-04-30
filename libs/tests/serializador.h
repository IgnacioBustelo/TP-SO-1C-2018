#ifndef TESTS_SERIALIZADOR_H_
#define TESTS_SERIALIZADOR_H_

#define HOST "127.0.0.1"
#define PORT 8080

typedef struct {
	int id;
	char* name;
	char* surname;
	float average;
} student_t;

student_t* student_create(int id, char* name, char* surname, float average);

size_t student_size(student_t* student);

void student_destroy(student_t* student);

package_t* serialize();

package_t* serialize_pretty();

void send_to_deserializer(int fd, package_t* package);

void send_to_deserializer_pretty(int fd, package_t* package);

#endif

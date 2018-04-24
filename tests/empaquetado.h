#ifndef TESTS_EMPAQUETADO_H_
#define TESTS_EMPAQUETADO_H_

typedef struct {
	int id;
	char* name;
	char* surname;
	float average;
} student_t;

student_t* create_student(int id, char* name, char* surname, float average);

size_t student_size(student_t* student);

void destroy_student(student_t* student);

void serialize();

#endif

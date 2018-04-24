#ifndef LIBS_DESERIALIZADOR_H_
#define LIBS_DESERIALIZADOR_H_

int receive_package(int fd, void* receiver, size_t size);

int receive_package_variable(int fd, void** receiver);

#endif

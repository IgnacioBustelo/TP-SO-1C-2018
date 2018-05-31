#include <commons/string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../libs/messenger.h"

int main(int argc, char* argv[]) {
	char *value = string_duplicate(argc == 1 ? "test" : argv[1]), *file_name = "store.txt";

	string_append(&value, "\n");

	int length = string_length(value);

	int fd = open(file_name, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR);

	truncate(file_name, length);

	if(fd == -1) {
		messenger_show("ERROR", "Problema abriendo el archivo %s", file_name);

		close(fd);

		free(value);

		exit(EXIT_FAILURE);
	}

	messenger_show("INFO", "Abierto el archivo '%s' con fd %d para escribir el valor '%s'", file_name, fd, value);

	void* memory = mmap(NULL, length, PROT_WRITE | PROT_READ | PROT_EXEC, MAP_SHARED, fd, 0);

	if(memory == MAP_FAILED) {
		messenger_show("ERROR", "Fallo mapeando a memoria");

		close(fd);

		free(value);

		exit(EXIT_FAILURE);
	}

	messenger_show("INFO", "Mapeo a memoria del archivo %s", file_name);

	memcpy(memory, value, length);

	messenger_show("INFO", "El valor %s se escribio en el archivo %s", (char*) memory, file_name);

	munmap(memory, length);

	close(fd);

	free(value);
}

.PHONY = clean

CC = gcc
SERIALIZADOR_DEPS = serializador.h ../libs/conector.h ../libs/serializador.h
SERIALIZADOR_SRC = $(SERIALIZADOR_DEPS:.h=.c)
DESERIALIZADOR_DEPS = deserializador.h ../libs/conector.h ../libs/deserializador.h
DESERIALIZADOR_SRC = $(DESERIALIZADOR_DEPS:.h=.c)
LIBS = -lcommons

serializador: $(SERIALIZADOR_SRC) $(SERIALIZADOR_DEPS)
	$(CC) -Wall $(SERIALIZADOR_SRC) -o serializador $(LIBS)
	
deserializador: $(DESERIALIZADOR_SRC) $(DESERIALIZADOR_DEPS)
	$(CC) -Wall $(DESERIALIZADOR_SRC) -o deserializador $(LIBS)

valgrind_serializador: serializador deserializador
	valgrind ./serializador
	
valgrind_deserializador: serializador deserializador
	valgrind ./deserializador

clean:
	rm -f *.o ../libs/*.o serializador deserializador
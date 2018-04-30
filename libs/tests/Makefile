.PHONY = clean

CC = gcc
TARGETS = serializador deserializador
LIBS = -lcommons

SERIALIZADOR_DEPS = serializador.h ../conector.h ../serializador.h
SERIALIZADOR_SRC = $(SERIALIZADOR_DEPS:.h=.c)

DESERIALIZADOR_DEPS = deserializador.h ../conector.h ../deserializador.h
DESERIALIZADOR_SRC = $(DESERIALIZADOR_DEPS:.h=.c)

all: $(TARGETS)

serializador: $(SERIALIZADOR_SRC) $(SERIALIZADOR_DEPS)
	$(CC) -Wall $(SERIALIZADOR_SRC) -o serializador $(LIBS)
	
deserializador: $(DESERIALIZADOR_SRC) $(DESERIALIZADOR_DEPS)
	$(CC) -Wall $(DESERIALIZADOR_SRC) -o deserializador $(LIBS)

clean:
	rm -f *.o $(TARGETS)
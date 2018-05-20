.PHONY = clean

CC = gcc -Wall
TARGETS = serializador deserializador msg_printfer
LIBS = -lcommons

SERIALIZADOR_DEPS = serializador.h ../conector.h ../serializador.h ../serializador_v2.h
SERIALIZADOR_SRC = $(SERIALIZADOR_DEPS:.h=.c)

DESERIALIZADOR_DEPS = deserializador.h ../conector.h ../deserializador.h
DESERIALIZADOR_SRC = $(DESERIALIZADOR_DEPS:.h=.c)

PRINTFER_DEPS = ../messenger.h ../mocks/printfer.h
PRINTFER_SRC = messenger_printfer.c ../mocks/printfer.c

all: $(TARGETS)

serializador: $(SERIALIZADOR_SRC) $(SERIALIZADOR_DEPS)
	$(CC) $(SERIALIZADOR_SRC) -o $@ $(LIBS)
	
deserializador: $(DESERIALIZADOR_SRC) $(DESERIALIZADOR_DEPS)
	$(CC) $(DESERIALIZADOR_SRC) -o $@ $(LIBS)
	
msg_printfer: $(PRINTFER_SRC) $(PRINTFER_DEPS)
	$(CC) $(PRINTFER_SRC) -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGETS)
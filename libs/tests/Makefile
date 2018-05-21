.PHONY = clean

CC = gcc -Wall
TARGETS = serializador deserializador printfer logger configurator
LIBS = -lcommons

SERIALIZADOR_DEPS = serializador.h ../conector.h ../serializador.h ../serializador_v2.h
SERIALIZADOR_SRC = $(SERIALIZADOR_DEPS:.h=.c)

DESERIALIZADOR_DEPS = deserializador.h ../conector.h ../deserializador.h
DESERIALIZADOR_SRC = $(DESERIALIZADOR_DEPS:.h=.c)

PRINTFER_DEPS = ../messenger.h ../mocks/printfer.h
PRINTFER_SRC = printfer.c ../mocks/printfer.c

LOGGER_DEPS = ../messenger.h ../logger.h
LOGGER_SRC = logger.c ../logger.c

CONFIGURATOR_DEPS = ../configurator.h ../messenger.h ../mocks/printfer.h
CONFIGURATOR_SRC = configurator.c ../configurator.c ../mocks/printfer.c

all: $(TARGETS)

serializador: $(SERIALIZADOR_SRC) $(SERIALIZADOR_DEPS)
	$(CC) $(SERIALIZADOR_SRC) -o $@ $(LIBS)
	
deserializador: $(DESERIALIZADOR_SRC) $(DESERIALIZADOR_DEPS)
	$(CC) $(DESERIALIZADOR_SRC) -o $@ $(LIBS)
	
printfer: $(PRINTFER_SRC) $(PRINTFER_DEPS)
	$(CC) $(PRINTFER_SRC) -o $@ $(LIBS)
	
logger: $(LOGGER_SRC) $(LOGGER_DEPS)
	$(CC) $(LOGGER_SRC) -o $@ $(LIBS)
	
configurator: $(CONFIGURATOR_SRC) $(CONFIGURATOR_DEPS)
	$(CC) $(CONFIGURATOR_SRC) -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGETS)
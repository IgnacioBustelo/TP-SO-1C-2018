CC = gcc -Wall

TARGET = cfg strg handshake entry_table_test
LIBS = -lcommons -lpthread

DEPS = ../../libs/messenger.h ../../libs/mocks/printfer.h
SRC = $(DEPS:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

cfg: DEPS += ../cfg_instancia.h ../../libs/configurator.h

strg: DEPS += ../storage.h

handshake: DEPS += ../coordinator_api.h ../globals.h ../../libs/chunker.h ../../libs/conector.h ../../libs/mocks/client_server.h ../../protocolo/protocolo_coordinador_instancia.h

entry_table_test: DEPS += ./utils.h ../entry_table.h ../globals.h

%: %.c $(DEPS) $(SRC)
	$(CC) $< $(SRC) -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGET)
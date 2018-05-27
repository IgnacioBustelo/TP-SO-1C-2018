CC = gcc -Wall

TARGET = cfg strg entry_table_test handshake receive_set notify_set entrega_2
LIBS = -lcommons -lpthread

API_DEPS = ../coordinator_api.h ../globals.h ../../libs/chunker.h ../../libs/conector.h ../../libs/mocks/client_server.h ../../protocolo/protocolo_coordinador_instancia.h
DEPS = ../../libs/messenger.h ../../libs/mocks/printfer.h
SRC = $(DEPS:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

cfg: DEPS += ../cfg_instancia.h ../../libs/configurator.h ../../libs/mocks/default_color.h

strg: DEPS += ../storage.h ../../libs/mocks/default_color.h

entry_table_test: DEPS += ./utils.h ../entry_table.h ../globals.h ../../libs/mocks/default_color.h

handshake: DEPS += $(API_DEPS)

receive_set: DEPS += $(API_DEPS)

notify_set: DEPS += $(API_DEPS)

entrega_2: DEPS += $(API_DEPS) ../main_mock.h ../../libs/configurator.h ../cfg_instancia.h ../instancia.h ../entry_table.h ../storage.h

%: %.c $(DEPS) $(SRC)
	$(CC) $< $(SRC) -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGET)
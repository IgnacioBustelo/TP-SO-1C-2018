CC = gcc -Wall

TARGET = cfg set store entry_table_test handshake receive_set notify_set die checkpoint_2
LIBS = -lcommons -lpthread

DEPS = ../../libs/messenger.h ../../libs/mocks/printfer.h
COORDINADOR_DEPS = coordinador_mock.h ../coordinator_api.h ../globals.h ../../libs/chunker.h ../../libs/conector.h ../../libs/mocks/client_server.h ../../protocolo/protocolo_coordinador_instancia.h

SRC = $(DEPS:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

cfg: DEPS += ../cfg_instancia.h ../../libs/configurator.h ../../libs/mocks/default_color.h

set: DEPS += ../storage.h ../../libs/mocks/default_color.h

store: DEPS += ../../libs/mocks/default_color.h

entry_table_test: DEPS += ./utils.h ../entry_table.h ../globals.h ../../libs/mocks/default_color.h

handshake: DEPS += $(COORDINADOR_DEPS)

receive_set: DEPS += $(COORDINADOR_DEPS)

notify_set: DEPS += $(COORDINADOR_DEPS)

die: DEPS += ../terminator.h ../../libs/mocks/color.h

checkpoint_2: DEPS += $(COORDINADOR_DEPS) ../bootstrap.h ../cfg_instancia.h ../../libs/configurator.h ../entry_table.h ../instancia.h ../storage.h ../terminator.h

%: %.c $(DEPS) $(SRC)
	$(CC) $< $(SRC) -o $@ $(LIBS)

clean:
	rm -f *.o *.txt $(TARGET)
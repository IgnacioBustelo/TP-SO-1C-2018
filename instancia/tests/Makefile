CC = gcc -Wall

TARGET = algorithms_test api cfg compact die dump final entry_table_test instancia recover storage store

LIBS = -lcommons -lpthread

DEPS = ../../libs/messenger.h ../../libs/mocks/printfer.h

COORDINADOR_DEPS = coordinador_mock.h ../coordinator_api.h ../globals.h ../../libs/chunker.h ../../libs/conector.h ../../libs/mocks/client_server.h ../../protocolo/protocolo_coordinador_instancia.h

SRC = $(DEPS:.h=.c)

all: $(TARGET)



algorithms_test:	DEPS += ./utils.h ../algorithms.h ../entry_table.h ../globals.h ../../libs/mocks/default_color.h

api:				DEPS += $(COORDINADOR_DEPS)

cfg:				DEPS += ../cfg_instancia.h ../../libs/configurator.h ../../libs/mocks/default_color.h

compact:			DEPS += ../algorithms.h ../compactation.h ../entry_table.h ../globals.h ../storage.h ../../libs/mocks/default_color.h utils.h

die:				DEPS += ../event_handler.h ../../libs/mocks/color.h

dump:				DEPS += ../dumper.h ../globals.h ../event_handler.h ../../libs/mocks/color.h ../../libs/mocks/default_color.h

entry_table_test:	DEPS += ./utils.h ../algorithms.h ../entry_table.h ../globals.h ../../libs/mocks/default_color.h

final:				DEPS += $(COORDINADOR_DEPS) ../algorithms.h ../cfg_instancia.h ../compactation.h ../../libs/configurator.h ../dumper.h ../event_handler.h ../entry_table.h ../instancia.h ../storage.h utils.h

instancia:			DEPS += $(COORDINADOR_DEPS) ../algorithms.h ../cfg_instancia.h ../compactation.h ../../libs/configurator.h ../dumper.h ../event_handler.h ../entry_table.h ../instancia.h ../storage.h utils.h

recover:			DEPS += ../dumper.h ../globals.h ../../libs/mocks/default_color.h

storage:			DEPS += ../globals.h ../storage.h ../../libs/mocks/default_color.h

store:				DEPS += ../dumper.h ../globals.h ../../libs/mocks/default_color.h



%: %.c $(DEPS) $(SRC)
	$(CC) $< $(SRC) -o $@ $(LIBS)

clean:
	rm -rf files backup dumped_keys $(TARGET)
	
.PHONY = all clean $(TARGET)
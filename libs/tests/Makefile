CC = gcc -Wall

TARGET = chunker client_server configurator list_csv logger printfer send_recv_many

LIBS = -lcommons -lpthread

DEPS = ../../libs/messenger.h ../mocks/printfer.h

SRC = $(DEPS:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

chunker: DEPS += ../chunker.h ../conector.h ../mocks/color.h ../mocks/client_server.h

client_server: DEPS += ../conector.h ../mocks/color.h ../mocks/client_server.h

configurator: DEPS += ../configurator.h ../mocks/default_color.h

list_csv: DEPS = ../../libs/messenger.h ../mocks/printfer.h ../mocks/default_color.h

logger: DEPS = ../../libs/messenger.h ../logger.h

printfer: DEPS = ../../libs/messenger.h ../mocks/printfer.h ../mocks/color.h

send_recv_many: DEPS += ../chunker.h ../conector.h ../mocks/color.h ../mocks/client_server.h

%: %.c $(DEPS) $(SRC)
	$(CC) $< $(SRC) -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGET)
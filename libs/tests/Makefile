CC = gcc -Wall

TARGET = printfer logger configurator client_server chunker
LIBS = -lcommons -lpthread

DEPS = ../../libs/messenger.h ../mocks/printfer.h
SRC = $(DEPS:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

printfer: DEPS = ../../libs/messenger.h ../mocks/printfer.h ../mocks/color.h

logger: DEPS = ../../libs/messenger.h ../logger.h

configurator: DEPS += ../configurator.h ../mocks/default_color.h

client_server: DEPS += ../conector.h ../mocks/color.h ../mocks/client_server.h

chunker: DEPS += ../chunker.h ../conector.h ../mocks/color.h ../mocks/client_server.h

%: %.c $(DEPS) $(SRC)
	$(CC) $< $(SRC) -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGET)
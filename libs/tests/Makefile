CC = gcc -Wall

TARGET = printfer logger configurator client_server
LIBS = -lcommons -lpthread

DEPS = ../../libs/messenger.h ../mocks/printfer.h
SRC = $(DEPS:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

%: %.c $(DEPS) $(SRC)
	$(CC) $< $(SRC) -o $@ $(LIBS)

logger: DEPS = ../../libs/messenger.h ../logger.h

configurator: DEPS += ../configurator.h

client_server: DEPS += ../mocks/client_server.h ../conector.h

clean:
	rm -f *.o $(TARGET)
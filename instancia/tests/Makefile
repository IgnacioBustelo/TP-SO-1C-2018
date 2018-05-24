CC = gcc -Wall

TARGET = cfg strg
LIBS = -lcommons

DEPS = ../../libs/messenger.h ../../libs/mocks/printfer.h
SRC = $(DEPS:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

%: %.c $(DEPS) $(SHARED) $(SRC)
	$(CC) $< $(SRC) $(SHARED:.h=.c) -o $@ $(LIBS)

cfg: DEPS += ../cfg_instancia.h ../../libs/configurator.h

strg: DEPS += ../storage.h

clean:
	rm -f *.o $(TARGET)
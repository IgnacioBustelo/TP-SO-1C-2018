CC = gcc -Wall

TARGET = cfg
LIBS = -lcommons

DEPS = 
SRC = $(DEPS:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

%: %.c $(SRC) $(DEPS)
	$(CC) $< $(SRC) -o $@ $(LIBS)

cfg: DEPS = ../cfg_instancia.h ../../libs/configurator.h ../../libs/messenger.h ../../libs/mocks/printfer.h

clean:
	rm -f *.o $(TARGET)
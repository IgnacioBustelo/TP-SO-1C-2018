CC = gcc -Wall

TARGET = cfg storage
LIBS = -lcommons

SHARED = ../../libs/messenger.h ../../libs/mocks/printfer.h
DEPS = 
SRC = $(DEPS:.h=.c) $(SHARED:.h=.c)

.PHONY = clean $(TARGET)

all: $(TARGET)

%: %.c $(SRC) $(DEPS) $(SHARED)
	$(CC) $< $(SRC) -o $@ $(LIBS)

cfg: DEPS = ../cfg_instancia.h ../../libs/configurator.h 

storage: DEPS = ../storage.h

clean:
	rm -f *.o $(TARGET)
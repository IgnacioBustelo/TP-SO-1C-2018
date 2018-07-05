CC = gcc
RM = rm -f

TARGET = instancia
SRC = $(wildcard *.c)
SRC += ../protocolo/protocolo_coordinador_instancia.c ../libs/conector.c ../libs/chunker.c ../libs/configurator.c ../libs/messenger.c ../libs/logger.c tests/utils.c
OBJ = $(SRC:.c=.o)
HEADER = $(SRC:.c=.h)

LIB = -lcommons -lpthread

all: $(TARGET)

%.o: %.c $(HEADER)
	$(CC) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIB)

debug: CC += -DDEBUG -g
debug: all

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)

.PHONY: all clean
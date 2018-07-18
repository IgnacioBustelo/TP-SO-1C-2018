CC = gcc
RM = rm -f

TARGET = coordinador
SRC = $(wildcard *.c) $(wildcard */*.c) ../libs/conector.c
OBJ = $(SRC:.c=.o)
HEADER = $(SRC:.c=.h) defines.h

LIB = -lcommons -lpthread

all: $(TARGET)

%.o: %.c $(HEADER)
	$(CC) -c $< -o $@

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIB)

debug: CC += -DDEBUG -g
debug: all

config: configs/$(CONFIG).cfg
	cp $< coordinador.cfg

clean:
	$(RM) $(OBJ)
	$(RM) $(TARGET)

.PHONY: all clean
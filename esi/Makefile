CC = gcc
RM = rm -f

TARGET = esi
SRC = $(wildcard *.c)
SRC += ../libs/conector.c ../libs/serializador.c
OBJ = $(SRC:.c=.o)
HEADER = $(SRC:.c=.h)

LIB = -lcommons -lparsi

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
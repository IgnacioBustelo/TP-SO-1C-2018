CC = gcc
RM = rm -f

SRC = $(wildcard *.c)
TARGET = $(SRC:%.c=%)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
CFLAGS = -g -Wall

all: $(TARGET)

test-consola: LIB += -lcommons -lreadline
test-consola: ../consola.c ../../libs/comando.c

%: %.c
	$(CC) $^ -o $@ $(LIB)

debug: CC += -DDEBUG -g
debug: all

clean:
	$(RM) $(TARGET)
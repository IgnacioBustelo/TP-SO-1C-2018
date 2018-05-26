CC = gcc
RM = rm -f

SRC = $(wildcard *.c)
TARGET = $(SRC:%.c=%)
OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
CFLAGS = -g -Wall

all: $(TARGET)

test-consola: LIB += -lcommons -lreadline
test-consola: ../consola.c ../../libs/comando.c

test-planificacion: LIB += -lcommons

test-deadlock: LIB += -lcommons -lreadline -lpthread
test-deadlock: ../consola.c ../planificador.c ../../libs/conector.c ../../libs/comando.c ../../libs/serializador.c ../../libs/deserializador.c ../config.c

%: %.c
	$(CC) $^ -o $@ $(LIB)

debug: CC += -DDEBUG -g
debug: all

clean:
	$(RM) $(TARGET)
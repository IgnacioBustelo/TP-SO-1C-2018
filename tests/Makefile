.PHONY = clean

CC = gcc
DEPS = serializador_funciones.h ../libs/conector.h ../libs/serializador.h ../libs/deserializador.h
SRC = serializador_funciones.c ../libs/conector.c ../libs/serializador.c ../libs/deserializador.c

empaquetado: empaquetado.c empaquetado.h ../libs/serializador.c ../libs/serializador.h
	$(CC) -Wall empaquetado.c ../libs/serializador.c -o empaquetado -lcommons

serializador: serializador_cliente.c $(SRC) $(DEPS)
	$(CC) -Wall serializador_cliente.c $(SRC) -o serializador -lcommons
	
deserializador: serializador_servidor.c $(SRC) $(DEPS)
	$(CC) -Wall serializador_servidor.c $(SRC) -o deserializador -lcommons

valgrind_serializador: serializador deserializador
	valgrind ./serializador
	
valgrind_deserializador: serializador deserializador
	valgrind ./deserializador

clean:
	rm -f *.o ../libs/*.o serializador deserializador
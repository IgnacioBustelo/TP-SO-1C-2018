TARGET = et_insert st_init
LIBS = -lcommons

.PHONY = clean $(TARGET)

all: $(TARGET)

et_insert: entry_table_insert.c ../instancia.h ../entry_table.c ../entry_table.h ../storage.h
	gcc entry_table_insert.c ../entry_table.c -o et_insert $(LIBS)

st_init: storage_init.c ../instancia.h ../storage.c ../storage.h
	gcc storage_init.c ../storage.c -o st_init $(LIBS)

clean:
	rm -f *.o $(TARGET)
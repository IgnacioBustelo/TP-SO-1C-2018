.PHONY = clean

TARGET = insert

all: $(TARGET)

insert: entry_table_insert.c ../instancia.h ../entry_table.c ../entry_table.h ../storage.h
	gcc entry_table_insert.c ../entry_table.c -o insert -lcommons
	
clean:
	rm -f *.o $(TARGET)
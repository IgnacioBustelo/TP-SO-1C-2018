TARGET = et_insert st_init st_set
LIBS = -lcommons

DEPS = utils.h ../instancia.h ../entry_table.h ../storage.h
ET_SRC = utils.c ../entry_table.c
ST_SRC = utils.c ../entry_table.c ../storage.c

.PHONY = clean $(TARGET)

all: $(TARGET)

et_insert: entry_table_insert.c $(ET_SRC) $(DEPS)
	gcc $< $(ET_SRC) -o $@ $(LIBS)

st_init: storage_init.c $(ST_SRC) $(DEPS)
	gcc $< $(ST_SRC) -o $@ $(LIBS)
	
st_set: storage_set.c $(ST_SRC) $(DEPS)
	gcc $< $(ST_SRC) -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGET)
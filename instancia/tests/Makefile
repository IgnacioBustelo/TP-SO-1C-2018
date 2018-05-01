TARGET = et_insert st_init st_set api_set_instancia api_set_coordinador
LIBS = -lcommons

DEPS = utils.h ../instancia.h ../entry_table.h ../coordinator_api.h ../storage.h ../../libs/conector.h ../../libs/serializador.h ../../libs/deserializador.h
ET_SRC = utils.c ../entry_table.c
ST_SRC = utils.c ../entry_table.c ../storage.c
API_SRC = utils.c ../coordinator_api.c ../../libs/conector.c ../../libs/deserializador.c ../../libs/serializador.c

.PHONY = clean $(TARGET)

all: $(TARGET)

et_insert: entry_table_insert.c $(ET_SRC) $(DEPS)
	gcc $< $(ET_SRC) -o $@ $(LIBS)

st_init: storage_init.c $(ST_SRC) $(DEPS)
	gcc $< $(ST_SRC) -o $@ $(LIBS)
	
st_set: storage_set.c $(ST_SRC) $(DEPS)
	gcc $< $(ST_SRC) -o $@ $(LIBS)
	
api_set_instancia: api_set_instancia.c $(API_SRC) $(DEPS)
	gcc $< $(API_SRC) -o $@ $(LIBS)
	
api_set_coordinador: api_set_coordinador.c $(API_SRC) $(DEPS)
	gcc $< $(API_SRC) -o $@ $(LIBS)

clean:
	rm -f *.o $(TARGET)
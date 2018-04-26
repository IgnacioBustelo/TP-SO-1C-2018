#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

void exit_gracefully(int status);

int max_data_saving_structure_entries;
int size_data_saving_structure_entries;

typedef struct {
	char* key;
	int entrie_number;
	int entrie_size;
}data_saving_structure_t;

typedef struct {
	int entrie_number;
	char* entrie_data;
}data_saving_t;

#endif

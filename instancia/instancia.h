#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

#define KEY_SIZE 12

#define FREE_SPACE_LEFT 10
#define NOT_FREE_SPACE_LEFT 11

void exit_gracefully(int status);



int actual_data_saving_structure_entries_occupied;

typedef struct {
	char* key;
	int entrie_number;
	int entrie_size;
}data_saving_administrative_structure_t;

typedef struct {
	int entrie_number;
	char* entrie_data;
}data_saving_structure_t;

#endif

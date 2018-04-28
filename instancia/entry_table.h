#ifndef INSTANCIA_ENTRY_TABLE_H_
#define INSTANCIA_ENTRY_TABLE_H_

#include <commons/collections/dictionary.h>

t_dictionary * entry_table;

int total_entries;
int entry_size;

int size_data_saving_structure_entries;

//Aca guardamos lo que recibimos del coordinador  RIP .
typedef struct {
	char key[40];
	void* value;
} key_value_t;

//Estructura que vamos a almacenar en el diccionario RIP .
typedef struct{
	int number;
	int size;
}entry_t;

int insert_entry(key_value_t key_value); //S

int update_entry(key_value_t key_value); //N

int delete_entry(char* key);

#endif

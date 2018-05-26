#include <stdlib.h>
#include <stdio.h>

#include "../entry_table.h"
#include "../globals.h"
#include "./utils.h"

int main()
{
	printf("Se levanta proceso \n\n");
	storage_setup_init(5,20);
	entry_table_init();
	printf("Cantidad de entradas: %d \n",get_total_entries());
	printf("Tamanio de entradas: %d \n",get_entry_size());

	entry_table_init();
	printf("Se inicializo correctamente la tabla de entradas \n\n");

	key_value_t* key_value=key_value_generator("X",21);

	printf("Creacion de key value de prueba \n");
	printf("El key value de prueba tiene clave: %s y valor: %s de %d bits \n\n",key_value->key,key_value->value,key_value->size);

	printf("La tabla de entradas tiene %d entradas libres \n\n",entries_left);

	 char* entries_left=entry_table_have_entries(key_value)==1?"SI":"NO";
	 printf("Hay entradas disponibles? %s \n",entries_left);

	 int next_entry = entry_table_next_entry(key_value);
	 printf("Proxima entrada disponible para almacenar: %d \n\n",next_entry);

	 char* result = entry_table_insert(next_entry,key_value)==false?"":"NO";

	 printf("El guardado %s fue exitoso \n\n",result);

	 printf("El table entry tiene %d registros \n\n",list_size(entry_table));

	 printf("Veo el primer registro \n");

	 entry_t * entry = (entry_t *)list_get(entry_table,0);
	 printf("Registro 0, KEY: %s, TAMANIO: %d y tiene INDICE STORAGE: %d \n",entry->key,entry->size,entry->number);

	 key_value=key_value_generator("y",25);
	 next_entry = entry_table_next_entry(key_value);
	 entry_table_insert(next_entry,key_value);

	 printf("\n\n\n");

	 printf("El table entry tiene %d registros \n\n",list_size(entry_table));

	 entry = (entry_t *)list_get(entry_table,1);
	 printf("Registro 1, KEY: %s, TAMANIO: %d y tiene INDICE STORAGE: %d \n",entry->key,entry->size,entry->number);

//	int next_entry=entry_table_next_entry(key_value);
//	printf("La proxima entrada libre es:%d",next_entry);
}

#include <stdlib.h>
#include <stdio.h>

#include "../entry_table.h"
#include "../globals.h"
#include "./utils.h"

int main()
{
//	printf("Se levanta proceso \n\n");
//	storage_setup_init(5,19);
//	entry_table_init();
//	printf("Cantidad de entradas: %d \n",get_total_entries());
//	printf("Tamanio de entradas: %d \n",get_entry_size());
//
//	entry_table_init();
//	printf("Se inicializo correctamente la tabla de entradas \n\n");
//
//	key_value_t* key_value=key_value_generator("X",21);
//
//	printf("Creacion de key value de prueba \n");
//	printf("El key value de prueba tiene clave: %s y valor: %s de %d bits \n\n",key_value->key,key_value->value,key_value->size);
//
//	printf("La tabla de entradas tiene %d entradas libres \n\n",entries_left);
//
//	 char* entries_left=entry_table_have_entries(key_value)==1?"SI":"NO";
//	 printf("Hay entradas disponibles? %s \n",entries_left);
//
//	 int next_entry = entry_table_next_entry(key_value);
//	 printf("Proxima entrada disponible para almacenar: %d \n\n",next_entry);
//
//	 char* result = entry_table_insert(next_entry,key_value)==false?"":"NO";
//
//	 printf("El guardado %s fue exitoso \n\n",result);
//
//	 printf("El table entry tiene %d registros \n\n",list_size(entry_table));
//
//	 printf("Veo el primer registro \n");
//
//	 entry_t * entry = (entry_t *)list_get(entry_table,0);
//	 printf("Registro 0, KEY: %s, TAMANIO: %d y tiene INDICE STORAGE: %d \n",entry->key,entry->size,entry->number);
//
//	 key_value=key_value_generator("y",25);
//	 next_entry = entry_table_next_entry(key_value);
//	 entry_table_insert(next_entry,key_value);
//
//	 printf("\n\n\n");
//
//	 printf("El table entry tiene %d registros \n\n",list_size(entry_table));
//
//	 entry = (entry_t *)list_get(entry_table,1);
//	 printf("Registro 1, KEY: %s, TAMANIO: %d y tiene INDICE STORAGE: %d \n",entry->key,entry->size,entry->number);
//
//	 key_value=key_value_generator("Z",10);
//	 	 next_entry = entry_table_next_entry(key_value);
//	 	 entry_table_insert(next_entry,key_value);
//
//	 	printf("\n\n\n");
//
//	 		 printf("El table entry tiene %d registros \n\n",list_size(entry_table));
//
//	 		 entry = (entry_t *)list_get(entry_table,2);
//	 		 printf("Registro 2, KEY: %s, TAMANIO: %d y tiene INDICE STORAGE: %d \n",entry->key,entry->size,entry->number);
//
//
//	 key_value=key_value_generator("y",25);
//	 entry_table_delete(key_value);
//	 printf("\n\n\n");
//	 entry_table_print_table();

//	 key_value=key_value_generator("T",25);
//	 next_entry = entry_table_next_entry(key_value);
//	 entry_table_insert(next_entry,key_value);
//	 printf("\n\n\n");
//	 entry_table_print_table();

		storage_setup_init(5,20);
		entry_table_init();
		key_value_t* key_value=key_value_generator("X",30);
		int next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);

		key_value=key_value_generator("Y",30);
		next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);

		key_value=key_value_generator("Z",15);
		next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);

		printf("\n");
		entry_table_print_table(); //X,Y,Z

		key_value=key_value_generator("Y",30);
		entry_table_delete(key_value);

		printf("\n");
		entry_table_print_table(); //X,Y

		key_value=key_value_generator("Y",30);
		next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);

		printf("\n");
		entry_table_print_table();

		key_value=key_value_generator("X",30);
		entry_table_delete(key_value);
//
		printf("\n");
		entry_table_print_table();


		key_value=key_value_generator("X",15);
		next_entry = entry_table_next_entry(key_value);
		entry_table_insert(next_entry,key_value);

		printf("\n");
		entry_table_print_table();

//		key_value=key_value_generator("M",15);
//		next_entry = entry_table_next_entry(key_value);
//		entry_table_insert(next_entry,key_value);

		printf("\n");
		entry_table_print_table();

		printf("\n\n");
		printf("La cantidad de entradas libres son: %d \n\n",entries_left);

		printf("Busco si la clave Y existe \n");

		entry_t * entry;

		entry = entry_table_get_entry("Y");

		printf("La clave Y apunta a la entrada %d y ocupa %d bits \n\n",entry->number,entry->size);

		printf("La cantidad de entradas atomicas es %d \n",entry_table_atomic_entries_count());
}


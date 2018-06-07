#include "algorithms.h"


void algorithm_circular_init(){
	algorithm_circular_pointer=0;
}

int algorithm_circular(t_list* entry_table,t_list* replaced_keys){
	if (algorithm_circular_pointer>=list_size(entry_table))
	{
		algorithm_circular_pointer=0;
	}
	while(algorithm_circular_pointer<list_size(entry_table))
	{

		algorithm_circular_pointer+=1;
	}
	return NULL;
}



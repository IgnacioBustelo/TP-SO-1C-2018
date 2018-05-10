#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../planificador.h"

typedef enum { FIFO, SJFCD, SJFSD, HRRN } t_scheduling_algorithm;

/* Local functions */

static esi_information* obtain_esi_information_by_id_test(int esi_fd, t_list* esi_bursts);

static esi_information* create_esi_information_test(int esi_id, int esi_numeric_name, double estimation);

static int schedule_esis_test(t_scheduling_algorithm algorithm, t_list* ready_queue, t_list* esi_bursts);

int main() {

	t_list* esi_bursts = list_create();

	list_add(esi_bursts, (void*)create_esi_information_test(1, 1, 3));
	list_add(esi_bursts, (void*)create_esi_information_test(2, 2, 3.5));
	list_add(esi_bursts, (void*)create_esi_information_test(3, 3, 4.7));
	list_add(esi_bursts, (void*)create_esi_information_test(4, 4, 1.2));

	t_list* ready_queue = list_create();

	int* esi_id1 = malloc(sizeof(int));
	*esi_id1 = 1;
	int* esi_id2 = malloc(sizeof(int));
	*esi_id2 = 2;
	int* esi_id3 = malloc(sizeof(int));
	*esi_id3 = 3;
	int* esi_id4 = malloc(sizeof(int));
	*esi_id4 = 4;
	list_add(ready_queue, (void*)esi_id1);
	list_add(ready_queue, (void*)esi_id2);
	list_add(ready_queue, (void*)esi_id3);
	list_add(ready_queue, (void*)esi_id4);

	schedule_esis_test(FIFO, ready_queue, esi_bursts);
	schedule_esis_test(SJFCD, ready_queue, esi_bursts);
	schedule_esis_test(SJFSD, ready_queue, esi_bursts);
	schedule_esis_test(HRRN, ready_queue, esi_bursts);

	return EXIT_SUCCESS;

}

static int schedule_esis_test(t_scheduling_algorithm algorithm, t_list* ready_queue, t_list* esi_bursts) {

	int* esi_fd;

	switch(algorithm) {

	case FIFO:
		esi_fd = (int*)ready_queue->head->data;
		break;

	case SJFCD:
	case SJFSD: {

		void* obtain_esi_information(void* esi_fd) {

			return (void*)obtain_esi_information_by_id_test(*(int*)esi_fd, esi_bursts);
		}

		bool comparator (void* esi_inf1, void* esi_inf2) {

			int last_estimated_burst1 = ((esi_information*)esi_inf1)->last_estimated_burst;
			int last_real_burst1 = ((esi_information*)esi_inf1)->last_real_burst;
			int last_estimated_burst2 = ((esi_information*)esi_inf2)->last_estimated_burst;
			int last_real_burst2 = ((esi_information*)esi_inf2)->last_real_burst;

			return last_estimated_burst1 - last_real_burst1 <= last_estimated_burst2 - last_real_burst2;

		}

		t_list* mapped_to_sort_list = list_map(ready_queue, obtain_esi_information);
	 	list_sort(mapped_to_sort_list, comparator);
	 	esi_fd = &((esi_information*)mapped_to_sort_list->head->data)->esi_id;
		break;
	}

	case HRRN: {

		void* obtain_esi_information(void* esi_fd) {

			return (void*)obtain_esi_information_by_id_test(*(int*)esi_fd, esi_bursts);
		}

		bool comparator (void* esi_inf1, void* esi_inf2) {

			int last_estimated_burst1 = ((esi_information*)esi_inf1)->last_estimated_burst;
			int last_estimated_burst2 = ((esi_information*)esi_inf2)->last_estimated_burst;

			return last_estimated_burst1 >= last_estimated_burst2;

		}

		t_list* mapped_to_sort_list = list_map(ready_queue, obtain_esi_information);
		list_sort(mapped_to_sort_list, comparator);
		esi_fd = &((esi_information*) mapped_to_sort_list->head->data)->esi_id;
		break;
	}

	}

	esi_information* esi_inf = obtain_esi_information_by_id_test(*esi_fd, esi_bursts);

	printf("El ESI seleccionado para ejecutar es el ESI %i\n", esi_inf->esi_numeric_name);

	return *esi_fd;
}

static esi_information* obtain_esi_information_by_id_test(int esi_fd, t_list* esi_bursts){

	bool equal_condition(void* esi_inf) {

		return ((esi_information*)esi_inf)->esi_id == esi_fd;
	}

	return list_find(esi_bursts, equal_condition);
 }

static esi_information* create_esi_information_test(int esi_id, int esi_numeric_name, double estimation) {

	esi_information* esi_inf = malloc(sizeof(esi_information));
	esi_inf->esi_id = esi_id;
	esi_inf->esi_numeric_name = esi_numeric_name;
	esi_inf->last_estimated_burst = estimation;
	esi_inf->last_real_burst = 0;
	esi_inf->waited_bursts = 0;
	return esi_inf;
}

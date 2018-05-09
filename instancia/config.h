#ifndef INSTANCIA_CONFIG_H_
#define INSTANCIA_CONFIG_H_

// Estructuras de datos

typedef enum {
	CIRC,
	LRU,
	BSU
} page_replacement_algorithm_t;

typedef struct {
	char*	coordinator_ip;
	int		coordinator_port;
	page_replacement_algorithm_t	page_replacement_algorithm;
	char*	mount_point;
	char*	instance_name;
	int		dump_interval;
} setup_t;

// Interfaz

setup_t	config_init();

#endif

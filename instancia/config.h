#ifndef INSTANCIA_CONFIG_H_
#define INSTANCIA_CONFIG_H_

typedef enum { CIRC, LRU, BSU } page_replacement_algorithm_t;

typedef struct {
	char* coordinator_ip;
	int coordinator_port;
	page_replacement_algorithm_t page_replacement_algorithm;
	char* mount_point;
	char* instance_name;
	int dump_interval;
} setup_t;

typedef struct{
	int total_entries;
	int entrie_size;
} administrative_structure_setup_t;

t_log*	init_log();

setup_t	init_config(t_log* logger);

void handshake_coordinador(int coordinador_fd, t_log* logger);

#endif

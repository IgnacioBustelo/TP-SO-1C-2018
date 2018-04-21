#ifndef COORDINADOR_COORDINADOR_H_
#define COORDINADOR_COORDINADOR_H_

void set_distribution(char* algorithm_name);

void init_log();

void init_config();

void check_config(char* key);

void *handle_connection(void *arg);

bool is_valid_process(int type);

void exit_gracefully(int status);

#endif

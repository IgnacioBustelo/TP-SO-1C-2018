/*
 * instancia.h
 *
 *  Created on: 19 abr. 2018
 *      Author: utnso
 */

#ifndef INSTANCIA_INSTANCIA_H_
#define INSTANCIA_INSTANCIA_H_

void check_config(char* key);

void init_log();

void init_config();

void set_distribution(char* algorithm_name);

void exit_gracefully(int status);

#endif /* INSTANCIA_INSTANCIA_H_ */

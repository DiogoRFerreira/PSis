#ifndef storage_H
#define storage_H

#include "structures.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void init_storage(variables*v);

int check_add_new_vector(variables v);

void add_message_vector_to_database(variables*v);

void get_write_position(variables v, int *index_for_list, int *index_for_vector);

void write_position(variables*v, int index_for_list, int index_for_vector, char*m);

void free_message_database(variables*v);


void get_index(int query_number, int*index_for_list, int*index_for_vector);

int check_valid_query_numbers(int total_messages, int query_number_o, int *query_number_s);

int validadate_username(variables v, char *user_verify);

#endif // storage_H

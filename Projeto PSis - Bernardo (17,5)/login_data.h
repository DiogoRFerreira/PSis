#ifndef LOGIN_DATA_H_INCLUDED
#define LOGIN_DATA_H_INCLUDED

#include "structures.h"
#include <stdio.h>
#include <stdlib.h>

void insert_client(variables*v,users*new_client);

void remove_client(variables*v,int new_sock);

void get_username(variables v, buffer_aux *b, int new_sock);

#endif // LOGIN_DATA_H_INCLUDED

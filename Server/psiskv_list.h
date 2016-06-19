#ifndef psiskv_list_h
#define psiskv_list_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct node {
    uint32_t key;
    int value_length;
    char * value;
    struct node * next;
} node;

void print_list();

int read_value(uint32_t key, char * p);

int add_value(uint32_t key, int value_length, char * value, int overwrite);

int delete_value(uint32_t key);

void build_backup(FILE * fp);

#endif /* psiskv_list_h */

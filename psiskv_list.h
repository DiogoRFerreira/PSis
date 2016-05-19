#ifndef psiskv_list_h
#define psiskv_list_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct node {
    uint32_t key;
    char * value;
    struct node * next;
} node;

void print_list();

uint32_t read_value(uint32_t key, char ** p);

uint32_t add_value(uint32_t key, char * value, int overwrite);

uint32_t delete_value(uint32_t key);

void build_backup(FILE * fp);

#endif /* psiskv_list_h */

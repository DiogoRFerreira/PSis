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

void print_list(node*head);

char * read_value(node * head, uint32_t key);

uint32_t add_value(node ** head, uint32_t key, char * value, int overwrite);

uint32_t delete_value(node ** head, uint32_t key);

#endif /* psiskv_list_h */

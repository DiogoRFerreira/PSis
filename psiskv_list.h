#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct node {
    uint32_t key;
    char * value;
    struct node * next;
} node;

char * read_value(node * head, uint32_t key);
void add_value(node ** head, uint32_t key, char * value) ;
int delete_value(node ** head, uint32_t key);

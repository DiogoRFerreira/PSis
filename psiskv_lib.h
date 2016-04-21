#ifndef psiskv_lib_h
#define psiskv_lib_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct msg_{
	uint8_t operation;
	uint32_t key;
	int value_length;
}message;

int kv_connect(char * kv_controler_ip,int kv_server_port);

void kv_close(int kv_descriptor);

int kv_write(int kv_descriptor, uint32_t key, char * value, int value_length);

int kv_read(int kv_descriptor, uint32_t key, char * value, int value_length);

int kv_delete(int kv_descriptor, uint32_t key);

#endif /* psiskv_lib_h */
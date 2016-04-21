#ifndef psiskv_server_lib_h
#define psiskv_server_lib_h

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct msg_{
	uint8_t operation;
	uint32_t key;
	int value_length;
}message;


int kv_server_listen(int kv_server_port);

int kv_server_accept(int fd);

int kv_server_read(int kv_descriptor);


#endif /* psiskv_server_lib_h */
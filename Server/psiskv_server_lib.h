#ifndef psiskv_server_lib_h
#define psiskv_server_lib_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "psiskv_server.h"
#include "psiskv_server_lib.h"
#include "psiskv_list.h"

typedef struct msg_{
	uint8_t operation;
	uint32_t key;
	int value_length;
}message;

extern pthread_mutex_t locklog;

int kv_server_listen(int kv_server_port);

int kv_server_accept(int fd);

int kv_server_read(int kv_descriptor);


#endif /* psiskv_server_lib_h */

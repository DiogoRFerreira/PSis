#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#define MQ_NAME "/aux_mq"
#include "messages.pb-c.h"
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
       
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>
#include <arpa/inet.h>    
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include <mqueue.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>   
#include <mqueue.h>

#include "structures.h"
#include "storage.h"
#include "login_data.h"
#include "log.h"

#include <semaphore.h>

#define PORT 3000
#define TRUE 1
#define FALSE 0
#define MAXCLIENTS 20

void * read_commands(void *arg);

void * client(void*arg);

void * read_messages_from_user(void *arg);

void * broadcast(void *arg);

void server(int sock_fd, int log_messages);


#endif // SERVER_H_INCLUDED

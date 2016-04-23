#include "psiskv_server_lib.h"
#include "psiskv_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>    /* POSIX Threads */

#define kv_server_port 2000

pthread_rwlock_t	rwlock;

void thread_handler ( void *ptr )
{
    int fd, close=0;           
    fd = (int) ptr;  // type cast to a pointer to int
    
    while(close==0){
		close=kv_server_read(fd);
	}
    
    pthread_exit(0); // exit
}

int main(){
	
	int fd, kv_descriptor;//File Descriptors
	pthread_t thread1;//Threads

	//Initialize Read/Write Mutex
	printf("Main, initialize the read write lock\n");
	pthread_rwlock_init(&rwlock, NULL);
	
	fd=kv_server_listen(kv_server_port);
	while(1){
		kv_descriptor = kv_server_accept(fd);
		// create threads 1
        if(pthread_create(&thread1, NULL, (void *) &thread_handler, (void *) kv_descriptor)) {
            printf("Error creating thread\n");
        }
		//pthread_join(thread1, NULL);
	}
}

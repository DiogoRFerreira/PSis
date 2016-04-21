#include "psiskv_server_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>    /* POSIX Threads */

#define kv_server_port 2000

void thread_handler ( void *ptr )
{
    int fd, close;           
    fd = (int) ptr;  /* type cast to a pointer to int */
    
    while(!close){
		close=kv_server_read(fd);
	}
    
    pthread_exit(0); /* exit */
}

int main(){
	
	int fd, kv_descriptor;//File Descriptors
	pthread_t thread1;//Threads
	//List
	node_t * head = NULL;
	
	fd=kv_server_listen(kv_server_port);
	while(1){
		kv_descriptor = kv_server_accept(fd);
		// create threads 1   
		pthread_create(&thread1, NULL, (void *) &thread_handler, (void *) &kv_descriptor);
		pthread_join(thread1, NULL);
	}
	
	exit(0);
}

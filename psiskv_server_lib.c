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

node * head = NULL;


int kv_server_listen(int kv_server_port){
	int fd;
	struct sockaddr_in addr;
	
	if((fd=socket(AF_INET,SOCK_STREAM,0))==-1){
		perror("Socket: ");//error
		return -1;
	}
	
	memset((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(kv_server_port);
	
	if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))==-1){
		perror("Bind: ");//error
		exit(-1);
	}
	
	if(listen(fd,5)==-1){
		perror("Listen: ");//error
		exit(-1);
	}
	
	return fd;
}

int kv_server_accept(int fd){
	printf("fd %d\n", fd);
	int newfd;
	struct sockaddr_in addr;
	socklen_t addrlen;
	addrlen=sizeof(addr);
	if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1){
		perror("Accept: ");
		return -1;
	}
	
	return newfd;
}

int kv_server_read(int kv_descriptor){
	message msg;
	long n;
	
	n=read(kv_descriptor, &msg, sizeof(msg));
	if(n<=0){
		perror("Read: ");
		return -1;
	}
	

    char * value=(char*)malloc(msg.value_length*sizeof(char));
    
	switch(msg.operation){
		case 1://Insert
			n=read(kv_descriptor, value, msg.value_length);
			if(n<=0){
				perror("Read: ");
				return -1;
			}
			//Insert value in the list
			//Critical Region
			pthread_rwlock_wrlock(&rwlock);
            head = add_value(head, msg.key, value);
            pthread_rwlock_unlock(&rwlock);
            printf(" Insert %s %u\n", value, msg.key);
			break;
		case 2://Retrieve
			//Search value in the list
			//Critical Region
			pthread_rwlock_rdlock(&rwlock);
            value=read_value(head, msg.key);
            pthread_rwlock_unlock(&rwlock);
            
            printf(" Retrieve %s %u\n", value, msg.key);
			n=write(kv_descriptor, value, msg.value_length);
			if(n<=0){
				perror("Write: ");
				return -1;
			}
			break;
		case 3://Delete
			//Apagar value e key da lista
            //Critical Region
            pthread_rwlock_wrlock(&rwlock);
            head=delete_value(head, msg.key);
            pthread_rwlock_unlock(&rwlock);
            
            printf(" Delete %u\n", msg.key);
			break;
	}
	print_list(head);
	
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "psiskv_server_lib.h"
#include "psiskv_list.h"

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
		return -1;
	}
	
	if(listen(fd,5)==-1){
		perror("Listen: ");//error
		return -1;
	}
	
	return fd;
}

int kv_server_accept(int fd){
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
            //head = add_value(head, msg.key, value);
			break;
		case 2://Retrieve
			//Search value in the list
            //value=read_value(head, msg.key)
			n=write(kv_descriptor, value, msg.value_length);
			if(n<=0){
				perror("Write: ");
				return -1;
			}
			break;
		case 3://Delete
			//Apagar value e key da lista
            //head=delete_value(head, msg.key)
			break;
	}
	
	return 0;
}

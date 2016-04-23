#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "psiskv_lib.h"

//Connect
int kv_connect(char * kv_controler_ip,int kv_server_port){
	int fd, n;
	struct sockaddr_in addr;
	
	fd=socket(AF_INET,SOCK_STREAM,0);//TCP socket
	if(fd==-1){
		perror("Socket: ");//error
		return -1;
	}
	//n=inet_aton(kv_controler_ip,&addr.sin_addr);
	
	memset((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	n=inet_aton(kv_controler_ip,&addr.sin_addr);
	if(n==0){
		printf("Invalid address");
		return -1;
	}
	addr.sin_port=htons(kv_server_port);
	
	n=connect(fd,(struct sockaddr*)&addr,sizeof(addr));
	if(n==-1){
		perror("Connect: ");//error
		return -1;
	}
	
	return fd;
}

//Close
void kv_close(int kv_descriptor){
	close(kv_descriptor);
}

//Insert
int kv_write(int kv_descriptor, uint32_t key, char * value, int value_length){
	message msg;
	msg.operation=1;
	msg.key=key;
	msg.value_length=value_length;
	long n;
	
	n=write(kv_descriptor, &msg, sizeof(msg));
	if(n<=0){
		perror("Write: ");
		return -1;
	}
	
	n=write(kv_descriptor, value, value_length);
	if(n<=0){
		perror("Write: ");
		return -1;
	}
	
	return 0;
}

//Retrieve
int kv_read(int kv_descriptor, uint32_t key, char * value, int value_length){
	long n;
	message msg;
	msg.operation=2;
	msg.key=key;
	msg.value_length=value_length;
	
	
	
	n=write(kv_descriptor, &msg, sizeof(msg));
	if(n<=0){
		perror("Write: ");
		return -1;
	}

	n=read(kv_descriptor, value, value_length);
	if(n<=0){
		perror("Read: ");
		return -1;
	}

	return 0;
}

//Delete
int kv_delete(int kv_descriptor, uint32_t key){
	long n;
	message msg;
	msg.operation=3;
	msg.key=key;
	msg.value_length=0;
	
	n=write(kv_descriptor, &msg, sizeof(msg));
	if(n<=0){
		perror("Write: ");
		return -1;
	}
	
	return 0;
}

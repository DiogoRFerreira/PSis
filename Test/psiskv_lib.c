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
    int fd , fd1;
    struct sockaddr_in addr;
    int port_dataserver;
    long n;

    fd = socket(AF_INET,SOCK_STREAM,0);//TCP socket
    if(fd==-1){
        perror("Socket: ");
        return -1;
    }
   // printf("--Socket fd: %d\n",fd);
    memset((void*)&addr,(int)'\0',sizeof(addr));
    addr.sin_family=AF_INET;
    n=inet_aton(kv_controler_ip,&addr.sin_addr);
    if(n==0){
        printf("Invalid address");
        return -1;
    }

    //Front Server Port
    addr.sin_port=htons(9999);

    //Connect to Front Server
    n = connect(fd,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1){
        perror("Connect: ");//error
        return -1;
    }
   // printf("--Connect fd: %d\n",fd);
    //printf("--Connected to FS\n");
    //Receive port from Front Server
    n=read(fd, &port_dataserver, sizeof(port_dataserver));
    if(n<0){
        perror("Read: ");
        return -1;
    }

    //Close socket with Front Server
    kv_close(fd);

    //Data Server Port
    addr.sin_port=htons(port_dataserver);

    fd1=socket(AF_INET,SOCK_STREAM,0);//TCP socket - necessário senão dá erro Bad file descriptor no connect
    if(fd1==-1){
        perror("Socket: ");
        return -1;
    }

    //Connect to Data Server
    n=connect(fd1,(struct sockaddr*)&addr,sizeof(addr));
    if(n==-1){
        perror("Connect: ");//error
        return -1;
    }
    return fd1;
}

//Close
void kv_close(int kv_descriptor){
	close(kv_descriptor);
}

//Insert
int kv_write(int kv_descriptor, uint32_t key, char * value, uint32_t value_length, int kv_overwrite){
	message msg;
    if (kv_overwrite == 0) msg.operation=1;
    else if(kv_overwrite == 1) msg.operation=2;
	msg.key=key;
	msg.value_length=value_length;
	long n;
    int returnvalue;

	//Send message with Operation, key value length
	n=write(kv_descriptor, &msg, sizeof(msg));

	if(n<0){
		perror("Write: ");
		return -1;
	}
	n=read(kv_descriptor, &returnvalue, sizeof(returnvalue));
    if(n<0){
        perror("Read: ");
        return -1;
    }
	//Send message with value
	n=write(kv_descriptor, value, value_length);
	if(n<0){
		perror("Write: ");
		return -1;
	}

    //Overwrite 1
    //returns 0, se concluir o overwrite ou seja a chave já existia no servidor
    //Overwrite 0
    //returns -2 se existir chave no servidor , retorna 0 em caso de sucesso
    n=read(kv_descriptor, &returnvalue, sizeof(returnvalue));
    if(n<0){
        perror("Read: ");
        return -1;
    }

	return returnvalue;
}

//Retrieve
int kv_read(int kv_descriptor, uint32_t key, char * value, uint32_t value_length){
	long n;
	message msg;
	msg.operation=3;
	msg.key=key;
	msg.value_length = value_length;
    int newlength=0;

    //Send message with Operation, key value length
	n=write(kv_descriptor, &msg, sizeof(msg));
	if(n<0){
		perror("Write: ");
		return -1;
	}
	int returnvalue;
	n=read(kv_descriptor, &returnvalue, sizeof(returnvalue));

    //Receives message with value_length of the stored value
    n=read(kv_descriptor, &newlength, sizeof(newlength));
    if(n<0){
        perror("Read: ");
        return -1;
    }
    
    if(newlength>0){
        //Receives message with value
        n=read(kv_descriptor, value, newlength);
        if(n<0){
                perror("Read: ");
                return -1;
        }
    }else{
        return -2;
    }

	return 0;
}

//Delete
int kv_delete(int kv_descriptor, uint32_t key){
	long n;
    int returnvalue;
	message msg;
	msg.operation=4;
	msg.key=key;
	msg.value_length=0;

	n=write(kv_descriptor, &msg, sizeof(msg));
	if(n<0){
		perror("Write: ");
		return -1;
	}
	n=read(kv_descriptor, &returnvalue, sizeof(returnvalue));
    n=read(kv_descriptor, &returnvalue, sizeof(returnvalue));
    if(n<0){
        perror("Read: ");
        return -1;
    }

	return returnvalue;
}

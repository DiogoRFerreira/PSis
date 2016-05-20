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

#define NUMBERLOGS 100 //number of writes in the log file before writing backup file again

//Log
int counter_log = 0;

pthread_mutex_t locklog;

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
	//printf("fd: %d\n", fd);
	int newfd;
	struct sockaddr_in addr;
	socklen_t addrlen;
	addrlen=sizeof(addr);
	if((newfd=accept(fd,(struct sockaddr*)&addr,&addrlen))==-1){
		perror("Accept: ");
		return -1;
	}
    //printf("newfd: %d\n", newfd);
	return newfd;
}

int kv_server_read(int kv_descriptor){
	message msg;
	long n;
    uint32_t returnvalue;
	
	n=read(kv_descriptor, &msg, sizeof(msg));
	if(n<=0){
		perror("Read: ");
		return -1;
	}
	
    printf("Chegou aqui\n");
    printf("---Msg: OP: %d key: %d value length: %d\n",msg.operation, msg.key, msg.value_length);
    char * value=(char*)malloc(msg.value_length*sizeof(char));
    
	if(msg.operation==1||msg.operation==2){//Insert
		int overwrite=0;
        n=read(kv_descriptor, value, msg.value_length);
        if(n<=0){
            perror("Read: ");
            return -1;
        }
        
        if(msg.operation==1) overwrite=0;//Insert without overwrite(0)--1
        else if(msg.operation==2) overwrite=1;//Insert with overwrite(1)--2
        //Insert value in the list
        //Critical Region
        //pthread_rwlock_wrlock(&rwlock);
        returnvalue=add_value(msg.key, value, overwrite);
        //pthread_rwlock_unlock(&rwlock);
        printf(" Insert %s %u\n", value, msg.key);
        //Sucess or not
        n=write(kv_descriptor, &returnvalue, sizeof(returnvalue));
        if(n<=0){
            perror("Write: ");
            return -1;
        }
    }
    if(msg.operation == 3){//Retrieve

        char *p = (char*)malloc(msg.value_length*sizeof(char));
        returnvalue=read_value(msg.key, &p);
        printf(" Retrieve %s %u\n", p, msg.key);
        
        //Envia o valor para ler
        n=write(kv_descriptor, &returnvalue, sizeof(returnvalue));
        if(n<=0){
            perror("Write: ");
            return -1;
        }
        //A chave existe envia
        if(returnvalue>0){
            n=write(kv_descriptor, p, sizeof(p));
            if(n<=0){
                perror("Write: ");
                return -1;
            }
        }
    }
    if(msg.operation==4){//Delete
        uint32_t returnvalue;
        //Apagar value e key da lista
        //Critical Region
        //pthread_rwlock_wrlock(&rwlock);
        printf("Delete value inside critical region\n");
        returnvalue=delete_value(msg.key);
        //pthread_rwlock_unlock(&rwlock);
        //Sucess or not
        n=write(kv_descriptor, &returnvalue, sizeof(returnvalue));
        if(n<=0){
            perror("Write: ");
            return -1;
        }
        printf(" Delete %u\n", msg.key);
	}

    //Log Part
    counter_log++;
    FILE * fp;
    if((fp=fopen("log.txt","a")) == NULL){
        perror("File: ");
    }else{
        printf("Colocando instrução no log!\n");
        if(msg.operation == 1|| msg.operation ==2){
			pthread_mutex_lock(&locklog);//Tem de escrever as duas vezes seguidas sempre
            fprintf(fp,"%d %u %u\n", msg.operation, msg.key, msg.value_length);
            fprintf(fp,"%s", value);
            pthread_mutex_unlock(&locklog);
        }else if(msg.operation == 4){
			pthread_mutex_lock(&locklog);
            fprintf(fp, "%d %u 0",msg.operation, msg.key);
            pthread_mutex_unlock(&locklog);
        }
        
        fclose(fp);
    }
    free(value);
    //-------------------
    int length = sizeof(int);
    int buff = counter_log;
    
	if(counter_log>=NUMBERLOGS){
		write(fd_pipeBackup[1],&buff, length);
	}
    
	print_list();
	
	return 0;
}


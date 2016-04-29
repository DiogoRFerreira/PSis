#include "psiskv_server_lib.h"
#include "psiskv_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>    /* POSIX Threads */

#define KV_PORT_FS 9999
#define KV_PORT_DS 10000

pthread_rwlock_t	rwlock;

//Front Server Threads
void client_handler ( void *ptr )//Recebe o connect e envia o porto do dataserver
{
     int fd, close;
     fd = (int) ptr;  // type cast to a pointer to int
     
     while(!close){
     close=kv_server_read(fd);
     }
    
}

void DSstate_handler ( void *ptr )//Verifica o estado do data server
{
    
}

//Data Server Threads

int main(){
	
	int fd, kv_descriptor;//File Descriptors
    pthread_t thread1,thread2;//Threads
    pid_t pid;
    char line[128];
    
    //Front Server & Data Server
    pid=fork();
	//Initialize Read/Write Mutex
	printf("Main, initialize the read write lock\n");
	pthread_rwlock_init(&rwlock, NULL);
	
    if(pid==0){//Filho - Data Server
        fd=kv_server_listen(KV_PORT_FS);
        while(1){
            kv_descriptor = kv_server_accept(fd);
            //Thread - Receber clientes
            if(pthread_create(&thread1, NULL, (void *) &client_handler, (void *) &kv_descriptor)) {
                printf("Error creating thread\n");
            }
            //Thread - Verificar o estado do Front Server
            //Thread - Escrever o backup e o log
        }
    }else{//Pai - Front Server
        fd=kv_server_listen(KV_PORT_DS);
        //Thread - Receber clientes
        if(pthread_create(&thread1, NULL, (void *) &client_handler, (void *) &kv_descriptor)) {
            printf("Error creating thread\n");
        }
        //Thread - Verificar estado do Data Server
        if(pthread_create(&thread2, NULL, (void *) &DSstate_handler, (void *) &kv_descriptor)) {
            printf("Error creating thread\n");
        }
        //Thread - Receber comandos do teclado
        while(1){
            fgets(line,128,stdin);
            printf("%s\n",line);
        }
    }
}

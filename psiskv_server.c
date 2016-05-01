#include "psiskv_server_lib.h"
#include "psiskv_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // SOCKET
#include <pthread.h>    // POSIX Threads
#include <signal.h>     // SIGPIPE

#define KV_PORT_FS 9999
#define KV_PORT_DS 10000

pthread_rwlock_t	rwlock;

//Front Server Threads
void FSclient_handler ( void * ptr )//Recebe o connect e envia o porto do dataserver
{
    int *fd;
    long n;
    fd = (int*) ptr;  // type cast to a pointer to int
    int port=KV_PORT_DS;
    printf("fd no client handler (FS): %d\n", *fd);
    
    //SIGPIPE signal
    void (*old_handler)(int);//interrupt handler
    if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR)exit(1);//error
    
    n=write(*fd, &port, sizeof(port));
    if(n<=0){
        perror("Write: ");
        return;
    }
}

void FSstate_handler ( void *ptr )//Verifica o estado do data server
{
    
}

//Data Server Threads
void DSclient_handler ( void *ptr )//Recebe o connect e envia o porto do dataserver
{
    int *fd, close=0;
    fd = (int*) ptr;  // type cast to a pointer to int
    
    //SIGPIPE signal
    void (*old_handler)(int);//interrupt handler
    if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR)exit(1);//error
    
    while(close==0){
        printf("Data Server Thread Server Read\n");
        close=kv_server_read(*fd);
        printf("close: %d\n", close);
    }
    
}

int main(){
	
	int fd1,fd2, kv_descriptor1,kv_descriptor2;//File Descriptors
    pthread_t thread1,thread2;//Threads
    pid_t pid;
    //char line[128];
    
    //Front Server & Data Server
    pid=fork();
    
    if(pid==0){//Filho - Data Server
        //Initialize Read/Write Mutex
        printf("Initialize the read write lock\n");
        pthread_rwlock_init(&rwlock, NULL);
        printf("Data Server Listen with pid %d\n",getpid());
        fd1=kv_server_listen(KV_PORT_DS);
        while(1){
            //Thread - Receber clientes
            kv_descriptor1 = kv_server_accept(fd1);
            printf("Data Server Accept\n");
            if(pthread_create(&thread1, NULL, (void *) &DSclient_handler, (void *) &kv_descriptor1)) {
                printf("Error creating thread\n");
            }
            //Thread - Verificar o estado do Front Server
            //Thread - Escrever o backup e o log
        }
    }else{//Pai - Front Server
        printf("Front Server Listen with pid %d\n",getpid());
        //Thread - Verificar estado do Data Server
        if(pthread_create(&thread2, NULL, (void *) &FSstate_handler, (void *) &kv_descriptor2)) {
            printf("Error creating thread\n");
        }
        fd2=kv_server_listen(KV_PORT_FS);
        while(1){
            //Thread - Receber clientes
            kv_descriptor2 = kv_server_accept(fd2);
            printf("Front Server Accept, kv_descriptor: %d\n",kv_descriptor2);
            if(pthread_create(&thread1, NULL, (void *) &FSclient_handler, (void *) &kv_descriptor2)) {
                printf("Error creating thread\n");
            }
        }
        //Thread - Receber comandos do teclado
        /*while(1){
            fgets(line,128,stdin);
            printf("%s\n",line);
        }*/
    }
}

#include "psiskv_server_lib.h"
#include "psiskv_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>     // SOCKET
#include <pthread.h>    // POSIX Threads
#include <signal.h>     // SIGPIPE
#include <sys/select.h> //SELECT

#define KV_PORT_FS 9999
#define KV_PORT_DS 10000

#define NUMBERLOGS 100 //number of writes in the log file before writing backup file again

pthread_rwlock_t rwlock, rwlock2;
pthread_mutex_t lock;

//Pipes
int fd_pipeFtoD[2];
int fd_pipeDtoF[2];

//
int fdDS,fdFS, kv_descriptorDS,kv_descriptorFS;//File Descriptors
pthread_t thread1, thread2, thread3, thread4, thread5 ,thread6;//Threads
pid_t pid;
FILE * fp;
char singleLine[150];
//Rescontrução da lista a partir do backup
uint32_t keybackup, keylog, value_lengthbackup, value_lengthlog;
uint8_t operationbackup;
uint32_t sucessbackup, sucesslog;

void mainFrontServer();
void mainDataServer();

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

void DSbackup_handler (void * ptr)
{
    FILE * fp;
    node * current;
    
    if((fp=fopen("backup1.txt","w"))==NULL){
        perror("File: ");
    }else{
        pthread_rwlock_rdlock(&rwlock);//Enquanto faz backup só podem ler da lista
        current=head;
        while(current->next!=NULL){
            fprintf(fp,"%u %d",current->key ,strlen(current->value));
            fprintf(fp,"%s",current->value);
            
        }
        fclose(fp);
        pthread_rwlock_unlock(&rwlock);
    }
}

void DSstate_handler (void * ptr)
{
	int rv;
	fd_set set;
	struct timeval timeout;
	pid_t buff;
	int length = sizeof(pid_t);
	
	if(fork()==0){	//Child - Verify State of Front
		while(1){
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			FD_ZERO(&set); // clear the set 
			FD_SET(fd_pipeFtoD[0], &set); // add our file descriptor to the set 
			rv = select(fd_pipeFtoD[0] + 1, &set, NULL, NULL, &timeout);
			if(rv == -1)
				perror("select"); // an error accured 
			else if(rv == 0){
				printf("timeout"); //a timeout occured 
				kill(buff, SIGKILL);
				if(fork()==0){
					mainFrontServer();
				}
			}
			else{
				read( fd_pipeFtoD[0], &buff, length); // there was data to read 
				printf("Read from F to D: %d\n",buff);
			 }
		}
	}else{		// Parent - Sends state of itself (Data) to Front (child)
		while(1){
			buff = getpid();
			write(fd_pipeDtoF[1],&buff, length);
			printf("Wrote from D to F: %d\n",buff);
			sleep(1);
		}
	}
}

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
    int rv;
	fd_set set;
	struct timeval timeout;
	pid_t buff;
	int length = sizeof(pid_t);

	if(fork()==0){	//Child - Verify State of Front
		while(1){
			timeout.tv_sec = 5;
			timeout.tv_usec = 0;
			FD_ZERO(&set); // clear the set 
			FD_SET(fd_pipeDtoF[0], &set); // add our file descriptor to the set 
			rv = select(fd_pipeDtoF[0] + 1, &set, NULL, NULL, &timeout);
			if(rv == -1)
				perror("select"); // an error accured 
			else if(rv == 0){
				printf("timeout"); // a timeout occured 
				kill(buff, SIGKILL);
				if(fork()==0){
					mainDataServer();
				}
			}else
				read(fd_pipeDtoF[0], &buff, length); // there was data to read 
				printf("Read from D to F: %d\n",buff);
		}
	}else{		// Parent - Sends state of itself (Data) to Front (child)
		while(1){
			buff = getpid();
			write(fd_pipeFtoD[1],&buff, length);
			printf("Wrote from F to D: %d\n", buff);
			sleep(1);
		}
	}
}

void FSinput_handler ( void *ptr )//Recebe inputs
{
    char buffer[128];
    while(1){
        fgets(buffer, 128, stdin);
        printf("%s",buffer);
        if(strncmp(buffer,"quit",4)==0){
            //quit command or press Ctr-C. In these cases both the Front and Data Servers will be orderly terminated.
            exit(0);
        }else if(strncmp(buffer,"print",5)==0){
            print_list(head);
        }
    }
}
// Main function of Front Server
void mainFrontServer(){
	printf("Front Server Listen with pid %d\n",getpid());
	
	//Thread - Verificar estado do Data Server
	if(pthread_create(&thread4, NULL, (void *) &FSstate_handler, (void *) NULL)){
		printf("Error creating thread\n");
	}
	
	//Thread - Receber comandos do teclado
	if(pthread_create(&thread5, NULL, (void *) &FSinput_handler, (void *) NULL)){
		printf("Error creating thread\n");
	}
	
	
	while(1){
		//Thread - Receber clientes
		kv_descriptorFS = kv_server_accept(fdFS);
		printf("Front Server Accept, kv_descriptor: %d\n",kv_descriptorFS);
		if(pthread_create(&thread6, NULL, (void *) &FSclient_handler, (void *) &kv_descriptorFS)) {
			printf("Error creating thread\n");
		}
	}
}
// Main function of Data Server
void mainDataServer(){
	//Thread - Verificar o estado do Front Server
	if(pthread_create(&thread1, NULL, (void *) &DSstate_handler, (void *) NULL)) {
		printf("Error creating thread\n");
	}
	//Thread - Escrever o backup e o log
	if(pthread_create(&thread2, NULL, (void *) &DSbackup_handler, (void *) NULL)) {
		printf("Error creating thread\n");
	}
	//Ler dados do backup e do log e
	if((fp=fopen("backup.txt","r"))==NULL){
		perror("File: ");
	}else{
		printf("Restaurando lista a partir do backup!\n");
		while(!feof(fp)){//Ler o backup
			fgets(singleLine,150, fp);
			sscanf(singleLine,"%u %u", &keylog, &value_lengthlog);
			printf("key: %u, value_length: %u",keylog, value_lengthlog);
			char *valuelog = (char*)malloc(value_lengthlog*sizeof(char));
			fgets(valuelog,value_lengthlog, fp);
			printf("Value: %s\n",valuelog);
			sucesslog=add_value(&head, keylog, valuelog, 1);
		}
		fclose(fp);
	}
	if((fp=fopen("log.txt","r"))==NULL){
		perror("File: ");
	}else{
		printf("Restaurando lista a partir do log!\n");
	   while(!feof(fp)){//Ler o log e inserir na lista
			fgets(singleLine, 150, fp);
			sscanf(singleLine,"%s %u %u",&operationbackup, &keybackup, &value_lengthbackup);
			if(operationbackup == 1 || operationbackup == 2){
				fgets(singleLine, 150, fp);
				printf("op: %u, key: %u, length: %u",operationbackup, keybackup, value_lengthbackup);
				printf("Value: %s\n",singleLine);
				if (operationbackup==1) sucessbackup=add_value(&head, keybackup, singleLine, 0);
				if (operationbackup==2) sucessbackup=add_value(&head, keybackup, singleLine, 1);
			}
			else if(operationbackup==4){
				sucessbackup=delete_value(&head, keybackup);
			}
		}
		fclose(fp);
	}
	
	printf("Data Server Listen with pid %d\n",getpid());
	
	while(1){
		//Thread - Receber clientes
		kv_descriptorDS = kv_server_accept(fdDS);
		printf("Data Server Accept\n");
		if(pthread_create(&thread3, NULL, (void *) &DSclient_handler, (void *) &kv_descriptorDS)) {
			printf("Error creating thread\n");
		}
	}
}

int main(){
    
    //Initialize Read/Writelocks & Mutex
    printf("Initialize the read write lock\n");
    pthread_rwlock_init(&rwlock, NULL);
    pthread_rwlock_init(&rwlock2, NULL);
    pthread_mutex_init(&lock, NULL);
    
    //Initialize Pipes
    if(pipe(fd_pipeDtoF)==-1)perror("Pipe Data to Front: ");
    if(pipe(fd_pipeFtoD)==-1)perror("Pipe Front to Data: ");
    
    //Initialize Sockets
    fdDS=kv_server_listen(KV_PORT_DS);
    fdFS=kv_server_listen(KV_PORT_FS);
    //Front Server & Data Server
    pid=fork();
    
    if(pid==0){//Filho - Data Server
        mainDataServer();
    }else{//Pai - Front Server
        mainFrontServer();
    }
    exit(0);
}

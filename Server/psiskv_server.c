#include "psiskv_server_lib.h"
#include "psiskv_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>     // SOCKET
#include <pthread.h>    // POSIX Threads
#include <signal.h>     // SIGNALS
#include <sys/select.h> // SELECT

#define KV_PORT_FS 9999
#define KV_PORT_DS 10000
#define NUM_THREADS 1000


pthread_rwlock_t rwlock, rwlock2;
pthread_mutex_t lock;

//Pids
pid_t dataserver;
pid_t frontserver;

//Pipes
int fd_pipeFtoD[2];
int fd_pipeDtoF[2];
int fd_pipeBackup[2];
int fd_pipeBackupReturn[2];

//File Descriptors
int fdDS = -1, fdFS;

//Try Bind DataServer
int iport = -1;

//Main prototype
void mainFrontServer();
void mainDataServer();

//Signal Handler
void sig_handler(int signo)
{
	//FILE * fplog; 
	int buff=1;
	int length = sizeof(int);
	if (signo == SIGINT) printf("Received SIGINT.\n");
	
	//Construir o Backup e fazer o unlink
	write(fd_pipeBackup[1],&buff, length);//build backup
	//Espera que o backup acabe de ser feito
	read(fd_pipeBackupReturn[0],&buff, length);
	//Sigkill do data server
	kill(dataserver, SIGKILL);
	
	exit(0);
}

//Data Server Threads
void DSclient_handler ( void *ptr )//Recebe o connect e envia o porto do dataserver
{
    int fd, close_var=0;
    fd = (int) ptr;  // type cast to a pointer to int

    while(close_var==0){
        close_var = kv_server_read(fd);
    }
   //printf("Client disconnected");
    close(fd);
    return;
}

void DSbackup_handler (void * ptr)
{
    FILE * fp ;
    FILE * fplog;
	int length = sizeof(int);
	int buff;
	while(1){
		if(read(fd_pipeBackup[0], &buff, length)){
			if((fplog=fopen("log.txt","w"))==NULL){//Apago os valores anteriores
				printf("file log.txt not found.\n");
			}else{ 
				if((fp=fopen("backup.txt","w"))==NULL){
					printf("file backup.txt not found.\n");
				}else{
					build_backup(fp);
					printf("Backup done.\n");
					write(fd_pipeBackupReturn[1],&buff, length);
				}
				fclose(fplog);
			}
		}
	}
}

void DSstate_handler (void * ptr)
{
	int rv;
	fd_set set;
	struct timeval timeout;
	pid_t buff;
	int length = sizeof(pid_t);
	

	while(1){
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		FD_ZERO(&set); // clear the set 
		FD_SET(fd_pipeFtoD[0], &set); // add our file descriptor to the set 
		rv = select(fd_pipeFtoD[0] + 1, &set, NULL, NULL, &timeout);
		if(rv == -1)
			perror("select"); // an error accured 
		else if(rv == 0){
			printf("Other component timed out.\n"); //a timeout occured 
			kill(frontserver, SIGKILL);
			if(fork()==0){
				mainFrontServer();
			}
		}
		else{
			read( fd_pipeFtoD[0], &buff, length); // there was data to read 
			frontserver=buff;
		 }
	}
}

void DS1state_handler (void * ptr)
{
	pid_t buff;
	int length = sizeof(pid_t);
	
	while(1){
		buff = getpid();
		write(fd_pipeDtoF[1],&buff, length);
		sleep(3);
	}
}


//Front Server Threads
void FSclient_handler ( void * ptr )//Recebe o connect e envia o porto do dataserver
{
    int fd;
    long n;
    fd = (int) ptr;  // type cast to a pointer to int
    int port=KV_PORT_DS+iport;
    
    n=write(fd, &port, sizeof(port));
    if(n<=0){
        perror("Write: ");
        return;
    }
    
    close(fd);
}

void FSstate_handler ( void *ptr )//Verifica o estado do data server
{
    int rv;
	fd_set set;
	struct timeval timeout;
	pid_t buff;
	int length = sizeof(pid_t);

	while(1){
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		FD_ZERO(&set); // clear the set 
		FD_SET(fd_pipeDtoF[0], &set); // add our file descriptor to the set 
		rv = select(fd_pipeDtoF[0] + 1, &set, NULL, NULL, &timeout);
		if(rv == -1)
			perror("select"); // an error accured 
		else if(rv == 0){
			printf("Other component timed out.\n"); //a timeout occured 
			kill(dataserver, SIGKILL);
			if(fork()==0){
				mainDataServer();
			}
		}else
			read(fd_pipeDtoF[0], &buff, length); // there was data to read 
			dataserver=buff;
	}
}
void FS1state_handler ( void *ptr )//Verifica o estado do data server
{
	pid_t buff;
	int length = sizeof(pid_t);
	
	while(1){
		buff = getpid();
		write(fd_pipeFtoD[1],&buff, length);
		sleep(3);
	}
	
}

void FSinput_handler ( void *ptr )//Recebe inputs
{
    char buffer[128];
    int length = sizeof(int);
    int buff=0;

    while(1){
		fgets(buffer, 128, stdin);
        if(strncmp(buffer,"quit",4)==0){
            //quit command or press Ctr+C. In these cases both the Front and Data Servers will be orderly terminated.
            //Construir backup e fazer o unlink
            printf("Server is going to terminate nicely...\n");
            write(fd_pipeBackup[1],&buff, length);//build backup
            read(fd_pipeBackupReturn[0],&buff, length);
            //Espera que o backup seja feito
            kill(dataserver, SIGKILL);
            exit(0);
        }
    }
}
// Main function of Front Server
void mainFrontServer(){
	int kv_descriptorFS;
	pthread_t thread4, thread5 , thread7;//Threads
	pthread_t thread;
	printf("Front Server initialized.\n");
	//Thread - Verificar estado do Data Server
	if(pthread_create(&thread4, NULL, (void *) &FSstate_handler, (void *) NULL)){
		printf("Error creating thread\n");
	}
	
	if(pthread_create(&thread7, NULL, (void *) &FS1state_handler, (void *) NULL)){
		printf("Error creating thread\n");
	}
	
	//Thread - Receber comandos do teclado
	if(pthread_create(&thread5, NULL, (void *) &FSinput_handler, (void *) NULL)){
		printf("Error creating thread\n");
	}
	
	//Signal Ctr-C
	if (signal(SIGINT, sig_handler) == SIG_ERR) printf("\nCan't catch SIGINT\n");
	printf("Front Server is waiting for clients . . .\n");
	while(1){
		//Thread - Receber clientes
		kv_descriptorFS = kv_server_accept(fdFS);
		if(pthread_create(&thread, NULL, (void *) &FSclient_handler, (void *) kv_descriptorFS)!=0) {
			printf("Error creating thread\n");
		}
	}
}
// Main function of Data Server
void mainDataServer(){
	char singleLine[150];
	FILE * fp;
	pthread_t thread1, thread2, thread8;//Threads
	pthread_t thread;
	int kv_descriptorDS;
	//Rescontrução da lista a partir do backup
	uint32_t keybackup, keylog, value_lengthbackup, value_lengthlog;
	int sucessbackup, sucesslog, operationlog;
	printf("Data Server initialized.\n");
	//Thread - Verificar o estado do Front Server
	if(pthread_create(&thread1, NULL, (void *) &DSstate_handler, (void *) NULL)) {
		printf("Error creating thread\n");
	}
	
	if(pthread_create(&thread8, NULL, (void *) &DS1state_handler, (void *) NULL)) {
		printf("Error creating thread\n");
	}
	
	//Thread - Escrever o backup e o log
	if(pthread_create(&thread2, NULL, (void *) &DSbackup_handler, (void *) NULL)) {
		printf("Error creating thread\n");
	}

	//Ler dados do Backup
	if((fp=fopen("backup.txt","r")) == NULL){
		printf("backup.txt not found.\n");
	}else{
		printf("Constructing list based on backup...\n");

		int i=1;
		char * valuebackup;
		while (fgets(singleLine, 150, fp)!=NULL){
        	if(i==1){
	        	sscanf(singleLine,"%u %u", &keybackup, &value_lengthbackup);
	        	valuebackup = (char*)malloc(value_lengthbackup*(sizeof(char)));
	        	i=2;
	        }else if(i==2){
	        	sscanf(singleLine,"%s",valuebackup);
	        	sucessbackup = add_value(keybackup,value_lengthbackup, valuebackup, 0);
	        	free(valuebackup);
	        	i=1;
	        }
		}
		fclose(fp);
	}

	//Ler dados do Log
	if((fp=fopen("log.txt","r"))==NULL){
		printf("log.txt not found.\n");
	}else{
		printf("Constructing list based on log...\n");
		char * valuelog;
	    while(fgets(singleLine, 150, fp)!= NULL){//Ler o log e inserir na lista
	   		//printf("%s",singleLine);
			sscanf(singleLine,"%u %u %u",&operationlog, &keylog, &value_lengthlog);
			//printf("OP: %u , K: %u, V: %u\n",operationlog, keylog, value_lengthlog);
			if(operationlog == 1 || operationlog == 2){
				fgets(singleLine, 150, fp);
			//	printf("op: %u, key: %u, length: %u",operationlog, keylog, value_lengthlog);
			//	printf("Value: %s\n",singleLine);
				valuelog = (char*)malloc(value_lengthlog*(sizeof(char)));
				sscanf(singleLine,"%s",valuelog);
				if (operationlog==1) sucesslog = add_value(keylog, value_lengthlog, valuelog, 0);
				if (operationlog==2) sucesslog = add_value(keylog, value_lengthlog, valuelog, 1);
				free(valuelog);
			}
			else if(operationlog==4){
				sucesslog = delete_value(keylog);
			}
		}
		fclose(fp);
	}
		printf("Data Server is waiting for clients . . .\n");
	while(1){
		//Thread - Receber clientes
		kv_descriptorDS = kv_server_accept(fdDS);
		//printf("New client accepted.\n");
		if(pthread_create(&thread, NULL, (void *) &DSclient_handler, (void *) kv_descriptorDS)!=0) {
			printf("Error creating thread\n");
		}
	}
}

int main(){
	pid_t pid;

    //Initialize Read/Writelocks & Mutex
    if(pthread_rwlock_init(&rwlock, NULL)!=0) printf("Error initializing rwlock\n"); 
    if(pthread_rwlock_init(&rwlock2, NULL)!=0) printf("Error initializing rwlock\n");
    if(pthread_mutex_init(&lock, NULL)!=0) printf("Error initializing mutex\n");
	if(pthread_mutex_init(&locklog, NULL)!=0) printf("Error initializing mutex\n");
    
    //Initialize Pipes
    if(pipe(fd_pipeDtoF)==-1)perror("Pipe Data to Front: ");
    if(pipe(fd_pipeFtoD)==-1)perror("Pipe Front to Data: ");
    if(pipe(fd_pipeBackup)==-1)perror("Pipe Backup: ");
    
    //Initialize Sockets
    while(fdDS==-1){
    	iport++;
    	fdDS=kv_server_listen(KV_PORT_DS+iport);
    }
    fdFS=kv_server_listen(KV_PORT_FS);
    if(fdFS==-1)exit(1);

    //Front Server & Data Server
    pid=fork();
    
    if(pid==0){//Filho - Data Server
		dataserver=getpid();
        mainDataServer();
        
    }else if(pid > 0) {//Pai - Front Server
		frontserver=getpid();
		mainFrontServer(); 

    }else printf("Error");
    
    exit(0);
}

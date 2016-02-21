#include "messages.pb-c.h"
#include <string.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <arpa/inet.h>   

#include <semaphore.h>
#include <pthread.h> 

#define PORT 3000
#define MESSAGE_LEN 100

typedef struct _variables_client{
	int sock_fd;
	struct sockaddr_in server_addr;
	struct sockaddr_in client_addr;
}variables_client;

void * send_requests(void *arg){
	variables_client v=(*((variables_client *)arg));
	int auxx;
    char *buff;
    char buffe[100];
    auxx=0;
    char id_min[100], id_max[100];
    char line[MESSAGE_LEN];
   /* char auxiliar[MESSAGE_LEN];*/
    char string_aux[MESSAGE_LEN];
    int cycle=1;
    
	while(auxx!=2){
		auxx=0;
		cycle=1;
		
	    /*---------------------------------------------------------------*/
		while(cycle==1){
			cycle=0;
			auxx=0;
			fgets(string_aux, MESSAGE_LEN, stdin);
			if((strncmp(string_aux, "DISC", 4)==0)||
				(strncmp(string_aux, "CHAT",4)==0)|| 
				(strncmp(string_aux, "LOGIN",5)==0)||
									(strncmp(string_aux, "QUERY",5)==0) ){
					if(strncmp(string_aux, "LOGIN", 5)==0){
						auxx=1;
						if(sscanf(string_aux, "%*s %s", line) != 1){
							printf("ERROR getting input command argument\n");
							cycle= 1;
						}
					}else{
						if(strncmp(string_aux, "DISC", 4)==0){
							auxx=2;
						}else{
							if(strncmp(string_aux, "CHAT",4)==0){
								auxx=3;
								if(sscanf(string_aux, "%*s %[^\n]\n", line) != 1){
									printf("ERROR getting input command argument\n");
									cycle=1;
								}
							}else{ /*message type QUERY*/
								auxx=4;
								if(sscanf(string_aux, "%*s %s %s",id_min, id_max) != 2){
									printf("ERROR getting input command argument\n");
									cycle= 1;
								}
							}
						}
					}
			}else{
				cycle= 1;
			}
		}

	    /*----------------------------------------------------------------*/
		CLIENTMESSAGE M=CLIENT__MESSAGE__INIT;
				
		M.type=auxx;
		
		if(auxx== 1){ /* request for login */	
			M.username=malloc(100*sizeof(char));
			strcpy(M.username, line);			
		}
						
		if(auxx== 3){ /* desire to send a chat message */	
			M.message=malloc(100*sizeof(char));
			strcpy(M.message, line);
		}
				
		if(auxx== 4){ /* query message */
			M.id_min=malloc(100*sizeof(char));
			M.id_max=malloc(100*sizeof(char));
			/*M.id_min=id_min;
			M.id_max=id_max;*/	
			strcpy(M.id_min, id_min);
			strcpy(M.id_max, id_max);		
		}
		
		buff = malloc(client__message__get_packed_size(&M));
		client__message__pack(&M, buff);
		send(v.sock_fd, buff, client__message__get_packed_size(&M),0);
		free(buff);
		perror("sendto");
		
				
		if(auxx== 2){/* disconnect request */
			close(v.sock_fd);
			exit(0);
		}
						
	}
	pthread_exit(NULL);
}

void* receive_messages(void *arg){
	char buff[10000];
	char*b;
	RESPONSE * resp_;
	variables_client v=(*((variables_client *)arg));
	fd_set readfds;
	
	while(1){
		size_t len = read(v.sock_fd, buff, 10000);
		if((len==-1)){
			/*happens when client doesn t login and
			 * desires to disconnect*/
			exit(1);
		}
		resp_=response__unpack(NULL, len, buff);
		if(resp_ == NULL){
			fprintf(stderr, "server no longer exists. please try again\n");
			exit(-1);
		}
		switch(resp_->type){
			case 1:
				printf("%s\n", resp_->login_valid);
				break;
			case 4:
				printf("%s\n", resp_->old_messages);
				break;
			case 3:
				printf("%s: %s\n", resp_->username_sender, resp_->m_forward);
				break;
		}
		
	
	}
}

int main(){
	int sock_fd;
	variables_client v;
	void*res;
	
char addr[20];
printf("choose addr\n");
fgets(addr, 20, stdin);
	
	/*welcome screen */
printf("------------------------------------------------------\n");
printf("Welcome to Bernardo-Naim's chat room!\n");
printf("You'll have the follwing commands available:\n");
printf("LOGIN\nDISC-to disconnect from chat\nCHAT - to send messages\n");
printf("QUERY id_min id_max - to ask for older messages\n");
printf("Please note that if you don't login you'll only me able to send");
printf("chat messages but you won't be able to receive from the other users.\n");
printf("\t query commands won't work also. We highly recommend you to login first!\n");
printf("\n\n ENJOY :)\n\n");
printf("------------------------------------------------------\n"); 
 
	/*envio de info para o server do tipo de request a fazer a seguir*/
	
	v.sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	perror("socket ");
	if(v.sock_fd == -1){
		exit(-1);
	}
	
	v.server_addr.sin_family = AF_INET;
	v.server_addr.sin_port = htons(PORT);
	/*"127.0.0.1"*/
	inet_aton(addr, & v.server_addr.sin_addr);
		
	/* connect to the server */
	if( connect(v.sock_fd, ( struct sockaddr *) &v.server_addr, sizeof(v.server_addr)) == -1){
		perror("connect ");
		exit(-1);
	}
	
	pthread_t responses, requests;
	
	pthread_create(&requests, NULL, send_requests, &v);
	pthread_create(&responses, NULL, receive_messages, &v);
	
	pthread_join(requests, &res);
	pthread_join(responses, &res);

    printf("OK\n");
    exit(0);
    
}

#define MQ_NAME "/aux_mq"
#include "messages.pb-c.h"
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
       
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>
#include <arpa/inet.h>    
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <string.h>

#include <mqueue.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>   
#include <mqueue.h>

#include "structures.h"
#include "storage.h"
#include "login_data.h"
#include "log.h"

#include <semaphore.h>

#define PORT 3000
#define TRUE 1
#define FALSE 0
#define MAXCLIENTS 20

variables v;

pthread_mutex_t mutex;


void * read_commands(void *arg){
	char string_aux[100];
	printf("commands available:\nQUIT-terminate the server\n");
	printf("LOG-print the log of all received messages\n\n");
	while(v.flag_quit!=1){
		fgets(string_aux, 100, stdin);
		if((((strlen(string_aux)-1)==4)||((strlen(string_aux)-1)==3))
							&&((strncmp(string_aux, "QUIT", 4)==0)||
							(strncmp(string_aux, "LOG",3)==0))){
			if(strncmp(string_aux, "QUIT", 4)==0){
				v.flag_quit=1;
			}else{
				/*v.flag_log=1;*/
				pthread_mutex_lock(&mutex);
				read_log();
				pthread_mutex_unlock(&mutex);
			}
		}
		
	}	
	pthread_exit(NULL);
}



void * client(void*arg){
	CLIENTMESSAGE *client_messag;
	RESPONSE Q=RESPONSE__INIT;
	buffer_aux b;
	char buff[100];
	char *buffe;
	int new_sock = *((int *)arg);
	users*new_client;
	
	int login_flag=0;
	int index_for_list, index_for_vector;
	int fd_fifo;
	char*query_resp;
	char username_[100];
	
	fd_fifo = open("/tmp/inter_thread_communication", O_WRONLY);
	if(fd_fifo == -1){
		perror("open");
		exit(-1);
	}
	  
	while(1){
					/*Acknowledge of th type os message that will be received*/
				
					size_t len1 = read(new_sock, buff, 100);
							
					/*extract message*/
					client_messag = client__message__unpack(NULL, len1, buff);
					if(client_messag == NULL){
						close(new_sock);
							if(login_flag==1){
								remove_client(&v, new_sock);
								v.database.n_clients=v.database.n_clients-1;
							}
						printf("client process was killed\n");
						pthread_exit(NULL);
					}
			
		/*------------------------------------------------------------------*/
					
					
						if((client_messag->type)==1){
						 /* LOGIN type of message */
							if(login_flag!=1){
								pthread_mutex_lock(&mutex);
								if(validadate_username(v, client_messag->username)==1){
									login_flag=1;
									v.database.n_clients=v.database.n_clients+1;
									new_client=malloc(sizeof(users));
									new_client->descriptor=new_sock;
									new_client->name=malloc(strlen(client_messag->username)*sizeof(char));
									strcpy(new_client->name, client_messag->username);
									insert_client(&v, new_client);
									write_log( 1, client_messag->username, "not necessary", &(v.log_messages));
									
									pthread_mutex_unlock(&mutex);
										
									Q.login_valid=(char*)malloc((strlen("Successfully loged in!\n")+1));
									strcpy(Q.login_valid, "Successfully loged in!\n");
									strcpy(username_, client_messag->username);
								}else{
									pthread_mutex_unlock(&mutex);
									Q.login_valid=(char*)malloc((strlen("Ups! :( the username you choose has already been taken...\n")+1));
									strcpy(Q.login_valid, "Ups! :( the username you choose has already been taken...\n");
								}
								Q.type=1;
							
								buffe=malloc(response__get_packed_size(&Q));
								
								response__pack(&Q, buffe);
								
								send(new_sock, buffe, response__get_packed_size(&Q), 0);
								
								free(Q.login_valid);
								free(buffe);
							}
						}
						
						if((client_messag->type)==2){ /* DISC(onect) type of message */
							pthread_mutex_lock(&mutex);
							write_log( 2, username_, "not necessary",&(v.log_messages));
							pthread_mutex_unlock(&mutex);
							close(new_sock);
							if(login_flag==1){
								pthread_mutex_lock(&mutex);
								remove_client(&v, new_sock);
								v.database.n_clients=v.database.n_clients-1;
								pthread_mutex_unlock(&mutex);
							}
							pthread_exit(NULL);
						}
						
						if((client_messag->type)==3){ /* CHAT type of message */
							pthread_mutex_lock(&mutex);
							write_log( 3, username_, client_messag->message, &(v.log_messages));
							pthread_mutex_unlock(&mutex);
											
							strcpy(b.m_forward, client_messag->message);							
							if(login_flag==1){
								pthread_mutex_lock(&mutex);
								get_username(v, &b, new_sock);
								pthread_mutex_unlock(&mutex);
							}					
							if( write(fd_fifo, &b, sizeof(b)) == -1){
								perror("write");
								exit(-1);
							}
							
							pthread_mutex_lock(&mutex);
							if(check_add_new_vector(v)==1){
								add_message_vector_to_database(&v); 
							}
							get_write_position(v, &index_for_list, &index_for_vector);
							write_position(&v, index_for_list, index_for_vector, client_messag->message);
							pthread_mutex_unlock(&mutex);
						}
							
						if((client_messag->type)==4){ /* QUERY type of message */
							if(login_flag==1){
								pthread_mutex_lock(&mutex);
								write_log( 4, username_, client_messag->message, &(v.log_messages));
								concatenate_query_response(v, atoi(client_messag->id_min), atoi(client_messag->id_max), &query_resp);
								pthread_mutex_unlock(&mutex);
								
								/*send query response*/
								Q.old_messages=(char*)malloc((strlen(query_resp)+1));
								strcpy(Q.old_messages, query_resp);
								Q.type=4;
								buffe=malloc(response__get_packed_size(&Q));
								response__pack(&Q, buffe);
								
								send(new_sock, buffe, response__get_packed_size(&Q), 0);
								
								
								free(Q.old_messages);
								free(buffe);
							}
						}					
	}
	pthread_exit(NULL);
}

void * read_messages_from_user(void *arg){
	pthread_t client_;
	int new_sock;
	sock addr, client_addr;
	fd_set readfds;	
	int sock_fd = *((int *)arg);

	
    int i=0;
    /*read messages loop*/
    while(1){
		FD_ZERO(&readfds);
		FD_SET(v.flag_quit, &readfds);
		FD_SET(sock_fd, &readfds);
		select(sock_fd +1, &readfds, NULL,NULL, NULL);
		
		if(FD_ISSET(v.flag_quit, &readfds)){
				close(sock_fd);
				pthread_exit(NULL);
		}
		
		if(FD_ISSET(sock_fd, &readfds)){
			new_sock = accept(sock_fd, NULL, NULL);
			perror("accept");
			i++;
			pthread_create(&client_, NULL, client, &new_sock);
		}
		
	}

}

void * broadcast(void *arg){
	char* buff;
	buffer_aux b;
	RESPONSE R=RESPONSE__INIT;
	fd_set readfds;
	users*aux_;
	
	R.type=3;/*type broadcast*/
	
	R.m_forward=(char*)malloc(100*sizeof(char));
	R.username_sender=(char*)malloc(100*sizeof(char));
	
	int fd_fifo;
	
	fd_fifo = open("/tmp/inter_thread_communication", O_RDONLY);
	if(fd_fifo == -1){
		perror("open");
		exit(-1);
	}
	
	while(1){
		FD_ZERO(&readfds);
		FD_SET(v.flag_quit, &readfds);
		FD_SET(fd_fifo, &readfds);
		select(fd_fifo+ v.flag_quit +1, &readfds, NULL,NULL, NULL);
		
		if(FD_ISSET(v.flag_quit, &readfds)){
				pthread_exit(NULL);
		}
		
		if(FD_ISSET(fd_fifo, &readfds)){
			/*read fifo*/
			read(fd_fifo, &b, sizeof(b));
			
			
			/* broadcast */
			strcpy(R.m_forward, b.m_forward);
			strcpy(R.username_sender, b.username_sender);
			buff=malloc(response__get_packed_size(&R));
			response__pack(&R, buff);
			
			pthread_mutex_lock(&mutex);
			aux_=(v.database.u.next);
			while(aux_!=NULL){
				send(aux_->descriptor, buff, response__get_packed_size(&R), 0);
				aux_=aux_->next;
			}
			pthread_mutex_unlock(&mutex);
			
			free(buff);
		}	
	}	
	free(R.m_forward);
	free(R.username_sender);
	pthread_exit(NULL);
}

void server(int sock_fd, int log_messages){
	pthread_t operator_keyboard, read_messages, broad_cast;

	void*res;
	
	init_storage(&v);
	v.database.u.next=NULL;
	/*v.flag_log=0;*/
	v.flag_quit=0;
	v.database.n_clients=0;
	v.log_messages=log_messages;
	
	pthread_mutex_init(&mutex, NULL);
	/*PTHREAD_MUTEX_INITIALIZER;*/
	
	if (mkfifo("/tmp/inter_thread_communication", 0600) != 0 ){
		perror("mkfifo ");
		exit(-1);
	}
	
	
	pthread_create(&operator_keyboard, NULL, read_commands, NULL);
	
	pthread_create(&read_messages, NULL, read_messages_from_user, &sock_fd);
	pthread_create(&broad_cast, NULL, broadcast, NULL);
		
	pthread_join(operator_keyboard, &res);
	pthread_join(read_messages, &res);
	pthread_join(broad_cast, &res);
	free_message_database(&v);
	unlink("/tmp/inter_thread_communication");
	write_log( 8, "doesnt matter", "doesnt matter", &v.log_messages);
	pthread_mutex_destroy(&mutex);
	exit(0);
	
}

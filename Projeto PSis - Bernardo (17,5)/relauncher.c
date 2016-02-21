#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include "messages.pb-c.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "server.h"

void Create_socket(int* sock_fd, sock*addr){
	(*sock_fd) = socket(AF_INET, SOCK_STREAM, 0);
	perror("socket ");
	if((*sock_fd) == -1){
		exit(-1);
	}
	
	addr->sin_family = AF_INET;
	addr->sin_port = htons(PORT);
	addr->sin_addr.s_addr = INADDR_ANY;
	
	return;
}


int main(){
	
	pid_t pid, status;
	sock addr, client_addr;
	char*u[1]={NULL};
	int sai=0;
	int sock_fd;
	int log_messages=1;
	
	/* create socket */ 
    Create_socket(&sock_fd, &addr);
    
    /*bind and listen*/
   	
	if(bind(sock_fd, (struct sockaddr *)  
		&addr, sizeof(addr))==-1){
		perror("bind");
	}
	
	
	if( listen(sock_fd, 1) == -1){
		perror("listen ");
		exit(-1);
	}
	
	
	write_log( 7, "doesnt matter", "doesnt matter", &log_messages);
	
	pid=fork();
	
	if(pid==0){
		server(sock_fd, log_messages);
		exit(0);

	}else{
		while(sai==0){
			wait(&status);
			if(WIFEXITED(status)!=1){
				unlink("/tmp/inter_thread_communication");
				get_lost_log_identifier(&log_messages);
				write_log( 5, "doesnt matter", "doesnt matter", &log_messages);
				if(fork() ==0){
					write_log( 6, "doesnt matter", "doesnt matter", &log_messages);
					server(sock_fd, log_messages);
				}
			
			}else{
				sai=1;
			}
		}
	}	
	exit(0);
}

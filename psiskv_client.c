#include "psiskv_lib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define kv_server_port 2000

int main(){

	char * kv_controler_ip = "192.168.3.17";
	char * value = (char*)malloc(15*sizeof(char));
	strcpy(value,"olaola");
	
	int value_length = (int)strlen(value), fd, error;
	
	uint32_t key = 35;
	
	while(1){
	//Connect
	fd=kv_connect(kv_controler_ip,kv_server_port);
	if(fd==-1) exit(1);

	//Insert
	error=kv_write(fd, key, value, value_length);
	if(error==-1) exit(2);
	//Retrieve
	error=kv_read(fd, key, value, value_length);
	if(error==-1) exit(3);
	//Delete
	error=kv_delete(fd, key);
	if(error==-1) exit(4);
	
	//Close
	kv_close(fd);
	}
	exit(0);
}


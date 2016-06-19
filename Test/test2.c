#include "psiskv_lib.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_VALUES 5
int main(){
	char * linha = (char*)malloc(100*sizeof(char));
	int i;

	//fork();
	//fork();

	int kv = kv_connect("192.168.1.8", 9999); 

	for ( i= 0; i < MAX_VALUES; i ++){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}

	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}

	for (i = 0; i < MAX_VALUES; i +=2){
		kv_delete(kv, i);
	}
	kv_close(kv);
	
}

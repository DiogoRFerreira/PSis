#include "psiskv_lib.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_VALUES 1000
int main(){
	char linha[100];
	uint32_t i;
	
	if(fork() == 0){
			
		int kv = kv_connect("192.168.1.3", 9999);

		for (i = 0; i < MAX_VALUES; i +=2){
			sprintf(linha, "%u", i);
			kv_write(kv, i , linha, strlen(linha)+1, 0);
		}
		kv_close(kv);


	}else{
		int kv = kv_connect("192.168.1.3", 9999);

		for (i = 1; i < MAX_VALUES; i +=2){
			sprintf(linha, "%u", i);
			kv_write(kv, i , linha, strlen(linha)+1, 0);
		}


		wait(NULL);
		printf("writing values\n");
		for (i = 1; i < MAX_VALUES; i +=2){
			sprintf(linha, "%u", i);
			kv_write(kv, i , linha, strlen(linha)+1, 0);
		}
			
		
		kv_close(kv);

	}
	
	
	
}
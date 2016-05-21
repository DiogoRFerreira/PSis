#include "psiskv_lib.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_VALUES 10
int main(){
	char linha[100];
	uint32_t i;

			
	int kv = kv_connect("192.168.1.3", 9999);
/************************************************/
	for (i = 0; i < MAX_VALUES; i++){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
getchar();
	for (i = 0; i < MAX_VALUES; i++){
		sprintf(linha, "%u", i+2);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	for (i = 0; i < MAX_VALUES; i++){
		kv_delete(kv, i);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
/************************************************/
	for (i = MAX_VALUES; i > 0; i--){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	for (i = MAX_VALUES; i > 0; i--){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	for (i = MAX_VALUES; i > 0; i--){
		kv_delete(kv, i);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
/************************************************/
	for (i = MAX_VALUES; i > 0; i-=2){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	for (i = MAX_VALUES; i > 0; i-=2){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	for (i = MAX_VALUES; i > 0; i-=2){
		kv_delete(kv, i);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
/************************************************/
	for (i = MAX_VALUES; i > 0; i-=3){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	for (i = MAX_VALUES; i > 0; i-=3){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	for (i = MAX_VALUES; i > 0; i-=3){
		kv_delete(kv, i);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
/************************************************/
	for (i = 0; i < MAX_VALUES; i+=4){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	for (i = 0; i < MAX_VALUES; i+=4){
		sprintf(linha, "%u", i+2);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	printf("QUASE\n");
getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}	
	printf("Aqui\n");
	getchar();
	for (i = 0; i < MAX_VALUES; i+=4){
		kv_delete(kv, i);
	}
	printf("LOL\n");
	getchar();
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	printf("FIM\n");
/************************************************/


	kv_close(kv);


}
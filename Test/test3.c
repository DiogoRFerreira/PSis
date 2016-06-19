#include "psiskv_lib.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_VALUES 10
int main(){
	char * linha = (char*)malloc(100*sizeof(char));
	int i;

	int kv = kv_connect("192.168.1.8", 9999);
/************************************************/
	for (i = 0; i < MAX_VALUES; i++){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
		//printf("QUASE1\n");
	for ( i = 0; i < MAX_VALUES; i++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
getchar();
	//printf("QUASE2\n");
	for (i = 0; i < MAX_VALUES; i++){
		sprintf(linha, "%u", i+2);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	getchar();
		//printf("QUASE3\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
		//printf("QUASE4\n");
	for (i = 0; i < MAX_VALUES; i++){
		kv_delete(kv, i);
	}
	getchar();
		//printf("QUASE5\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
		//printf("QUASE6\n");
/************************************************/
	for (i = MAX_VALUES; i > 0; i--){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
		//printf("QUASE7\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
		//printf("QUASE8\n");
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
		//printf("QUASE9\n");
	for (i = MAX_VALUES; i > 0; i--){
		kv_delete(kv, i);
	}
	getchar();
		//printf("QUASE10\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
		//printf("QUASE11\n");
/************************************************/
	for (i = MAX_VALUES; i > 0; i-=2){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
	//printf("QUASE12\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	//printf("QUASE13\n");
	for (i = MAX_VALUES; i > 0; i-=2){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	getchar();
	//printf("QUASE14\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	//printf("QUASE15\n");
	for (i = MAX_VALUES; i > 0; i-=2){
		kv_delete(kv, i);
	}
	getchar();
	//printf("QUASE16\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	//printf("QUASE17\n");
/************************************************/
	for (i = MAX_VALUES; i > 0; i-=3){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
	//printf("QUASE18\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	//printf("QUASE19\n");
	for (i = MAX_VALUES; i > 0; i-=3){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	getchar();
	//printf("QUASE20\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	//printf("QUASE21\n");
	for (i = MAX_VALUES; i > 0; i-=3){
		kv_delete(kv, i);
	}
	getchar();
	//printf("QUASE22\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	//printf("QUASE23\n");
/************************************************/
	for (i = 0; i < MAX_VALUES; i+=4){
		sprintf(linha, "%u", i);
		kv_write(kv, i , linha, strlen(linha)+1, 0);
	}
	getchar();
	//printf("QUASE24\n");
	for ( i = 0; i < MAX_VALUES; i ++){
		if(kv_read(kv, i , linha, 1000) == 0){
			printf ("key - %10u value %s\n", i, linha);
		}
	}
	getchar();
	//printf("QUASE25\n");
	for (i = 0; i < MAX_VALUES; i+=4){
		sprintf(linha, "%u", i+2);
		kv_write(kv, i , linha, strlen(linha)+1, 1);
	}
	//printf("QUASE\n");
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

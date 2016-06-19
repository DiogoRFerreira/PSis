#include "psiskv_lib.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_VALUES 5
int main(){
	char * linha = (char*)malloc(100*sizeof(char));
	uint32_t i;
	int res=-2;
	if(fork() == 0){
		int kv = kv_connect("127.0.0.1", 9999);
				printf("\n");
		printf("Inserting key [1..5] values [1..5]\n");
				printf("\n");
		for (i = 1; i < 6; i ++){
			sprintf(linha, "%u", i);
			kv_write(kv, i , linha, strlen(linha)+1, 0);
			printf("-- W --  		%u	|	%s		pid: %d		OW: %d\n", i,linha, getpid(), 0);
		}
		kv_close(kv);


	}else{
		int kv = kv_connect("127.0.0.1", 9999);
		printf("\n");
		printf("Inserting key [1..10] values [1..10]\n");
		printf("\n");
		for (i =1; i < 11; i ++){
			sprintf(linha, "%u", i);
			kv_write(kv, i , linha, strlen(linha)+1, 1);
			printf("-- W --  		%u	|	%s		pid: %d		OW: %d\n", i,linha,getpid(),1);
		}
		printf("\n------------------------------------------------------------------------------\n\n");
		
		
		wait(NULL);
		printf("\n");
		printf("Reading key [0..10] \n");
		getchar();
		printf("\n");
		for (i = 0; i < 11; i ++){
            res = kv_read ( kv, i , linha, 1000 );
            if ( res >= 0 )
            {
                printf("-- R --  		%u	|	%s\n", i,linha);
            }
            else
            {
                printf ( "Key[%u] doesn't exist. \n", i );
            }
		}
		printf("\n------------------------------------------------------------------------------\n\n");
		printf("Deleting key [1..10]	--> odd values\n");
		getchar();
		printf("\n");
		for (i = 1; i < 11; i=i+2){
            kv_delete(kv, i);
            printf("-- D --  	%u			(if exists)\n", i);
            
		}
		printf("\n------------------------------------------------------------------------------\n\n");
		printf("Reading key [1..10]\n");
		getchar();
		printf("\n");
		for (i = 1; i < 11; i ++){
            res = kv_read ( kv, i , linha, 1000 );
            if ( res >= 0 )
            {
                printf("-- R --  		%u	|	%s\n", i,linha);
            }
            else
            {
                printf ( "Key[%u] doesn't exist.\n", i);
            }
		}
		
		
		printf("\n------------------------------------------------------------------------------\n\n");
		printf("Deleting key [2..10]	--> even values\n");
		getchar();
		printf("\n");
		for (i = 2; i < 11; i=i+2){
            kv_delete(kv, i);
            printf("-- D --  	%u			(if exists)\n", i);
            
		}
		printf("\n------------------------------------------------------------------------------\n\n");
		printf("Reading key [1..10]	\n");
		getchar();
		printf("\n");
		for (i = 1; i < 11; i ++){
            res = kv_read ( kv, i , linha, 1000 );
            if ( res >= 0 )
            {
                printf("-- R --  		%u	|	%s\n", i,linha);
            }
            else
            {
                printf ( "Key[%u] doesn't exist.\n", i);
            }
		}
		kv_close(kv);
		}
		printf("\n");
		printf("Inserting concurrently with OW 0 & 1 key [1..10] values [1..10] & [10..100]\n");
		getchar();
		printf("\n");
		if(fork() == 0){
			
			int kv = kv_connect("127.0.0.1", 9999);
			
			for (i = 1; i < 11; i ++){
				sprintf(linha, "%u", i);
				kv_write(kv, i , linha, strlen(linha)+1, 0);
				printf("-- W --  		%u	|	%s		pid: %d		OW: %d\n", i,linha, getpid(), 0);
			}
			kv_close(kv);
		}else{
			int kv = kv_connect("127.0.0.1", 9999);
			for (i = 1; i < 11; i ++){
				sprintf(linha, "%u", i*10);
				kv_write(kv, i , linha, strlen(linha)+1, 1);
				printf("-- W --  		%u	|	%s		pid: %d		OW: %d\n", i,linha, getpid(),1);
			}
			printf("\n------------------------------------------------------------------------------\n\n");
			wait(NULL);
			
			printf("Reading key [1..10]\n");
			getchar();
			printf("\n");
			for (i = 1; i < 11; i ++){
				res = kv_read ( kv, i, linha, 1000 );
				if ( res >= 0 )
				{
					printf("-- R --  		%u	|	%s\n", i,linha);
				}
				else
				{
					printf ( "Key[%u] doesn't exist.\n", i);
				}
			}
		printf("\n------------------------------------------------------------------------------\n\n");
		printf("Deleting key [1..10]\n");
		getchar();
		printf("\n");
		for (i = 1; i < 11; i++){
            kv_delete(kv, i);
            printf("-- D --  	%u			(if exists)\n", i);
		}
		
		
		printf("\n------------------------------------------------------------------------------\n\n");
		printf("Reading key [1..10]\n");
		getchar();
		printf("\n");
			for (i = 1; i < 11; i ++){
				res = kv_read ( kv, i, linha, 1000 );
				if ( res >= 0 )
				{
					printf("-- R --  		%u	|	%s\n", i,linha);
				}
				else
				{
					printf ( "Key[%u] doesn't exist. \n", i);
				}
			}
			printf("\n------------------------------------------------------------------------------\n\n");
		kv_close(kv);
		}
		printf("\n");
		printf("Inserting & reading concurrently the same keys[30..40] values[300..400]\n");
			getchar();
		printf("\n");
		if(fork() == 0){
			int kv = kv_connect("127.0.0.1", 9999);
			
			for (i = 30; i < 41; i ++){
				sprintf(linha, "%u", i*10);
				kv_write(kv, i , linha, strlen(linha)+1, 0);
				printf("-- W --  		%u	|	%s		pid: %d		OW: %d l:%d\n", i,linha, getpid(), 0, strlen(linha));
			}
			kv_close(kv);
		}else{
			int kv = kv_connect("127.0.0.1", 9999);

			for (i = 30; i < 41; i ++){
				res = kv_read ( kv, i, linha, 1000 );
				if ( res >= 0 )
				{
					printf("-- R --  		%u	|	%s\n", i,linha);
				}
				else
				{
					printf ( "Key[%u] doesn't exist.\n", i);
				}
			}
			printf("\n------------------------------------------------------------------------------\n\n");
			wait(NULL);
		printf("Reading keys[0..45]\n");
		getchar();
		printf("\n");
			for (i = 0; i < 46; i ++){
				res = kv_read ( kv, i, linha, 1000 );
				if ( res >= 0 )
				{
					printf("-- R --  		%u	|	%s\n", i,linha);
				}
				else
				{
					printf ( "Key[%u] doesn't exist.\n", i);
				}
			}
		printf("Deleting keys[0..50]\n");
			getchar();
		printf("\n");
			printf("\n------------------------------------------------------------------------------\n\n");
			for (i = 0; i < 50; i++){
				kv_delete(kv, i);
				printf("-- D --  	%u			(if exists)\n", i);
				
			}
			printf("\n------------------------------------------------------------------------------\n\n");
		printf("Reading keys[0..50]\n");
			getchar();
		printf("\n");
						for (i = 0; i < 51; i ++){
				res = kv_read ( kv, i, linha, 1000 );
				if ( res >= 0 )
				{
					printf("-- R --  		%u	|	%s\n", i,linha);
				}
				else
				{
					printf ( "Key[%u] doesn't exist. \n", i);
				}
			}
			printf("\n------------------------------------------------------------------------------\n\n");
			printf("Inserting keys[0..5] values[10..15]\n");
				getchar();
			printf("\n");
			for (i = 0; i < 6; i ++){
				sprintf(linha, "%u", i+10);
				kv_write(kv, i , linha, strlen(linha)+1, 0);
				printf("-- W --  		%u	|	%s		pid: %d		OW: %d l:%d\n", i,linha, getpid(), 0,strlen(linha));
			}
			printf("\n------------------------------------------------------------------------------\n\n");
			printf("Overwriting keys[0..10] values [0..-10] \n");
				getchar();
				getchar();
			printf("\n");
			for (i = 0; i < 11; i ++){
				sprintf(linha, "%u", i+200);
				kv_write(kv, i , linha, strlen(linha)+1, 1);
				printf("-- W --  		%u	|	%s		pid: %d		OW: %d l:%d\n", i,linha, getpid(), 1,strlen(linha));
			}
			printf("\n------------------------------------------------------------------------------\n\n");
			printf("Reading keys[0..10]\n");
				getchar();
			printf("\n");
			for (i = 0; i < 11; i ++){
				res = kv_read ( kv, i, linha, 1000 );
				if ( res >= 0 )
				{
					printf("-- R --  		%u	|	%s\n", i,linha);
				}
				else
				{
					printf ( "Key[%u] doesn't exist. \n", i);
				}
			}
			
			printf("\n------------------------------------------------------------------------------\n\n");
			printf("Deleting keys[0..10]\n");
				getchar();
			printf("\n");
			for (i = 0; i < 10; i++){
				kv_delete(kv, i);
				printf("-- D --  	%u			(if exists)\n", i);
				
			}
			printf("\n------------------------------------------------------------------------------\n\n");
			printf("Reading keys[0..10]\n");
				getchar();
			printf("\n");
			for (i = 0; i < 11; i ++){
				res = kv_read ( kv, i, linha, 1000 );
				if ( res >= 0 )
				{
					printf("-- R --  		%u	|	%s\n", i,linha);
				}
				else
				{
					printf ( "Key[%u] doesn't exist.\n", i);
				}
			}
			
			kv_close(kv);
			printf("\n--------------------	List shall be empty now.	--------------------------------\n\n");

	}
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"

void write_log(int type, char* username, char* message, int *number){
	FILE * fp = fopen("log.txt", "a");
	
	switch(type){
		case 1:
			fprintf((fp), "%d : At %d the user %s logged in\n", *number, (int)time(NULL), username);
			break;
		case 2:
			fprintf((fp), "%d : At %d the user %s logged out\n", *number, (int)time(NULL), username);
			break;
		case 3:
			fprintf((fp), "%d : At %d the user %s wrote:%s\n", *number, (int)time(NULL), username, message);
			break;
		case 4:
			fprintf((fp), "%d : At %d the user %s requested for query\n", *number, (int)time(NULL), username);
			break;
		case 5:
			fprintf((fp), "%d : At %d entire process was killed\n", *number, (int)time(NULL));
			break;
		case 6:
			fprintf((fp), "%d : At %d chat service was relaunched\n", *number, (int)time(NULL));
			break;
		case 7:
			fprintf((fp), "%d : At %d chat service was started\n", *number, (int)time(NULL));
			break;
		case 8:
			fprintf((fp), "%d : At %d chat service shut down\n", *number, (int)time(NULL));
			break;
	}
	*number=(*number)+1;
	fclose(fp);
	return;
}

void read_log(){
	char line[200];
	FILE*fp;
	
	fp = fopen("log.txt", "r");
	
	if ( fp == NULL ) {
		printf("There is no activity yet...\n");
		exit(-1);
    }else{
		while(fgets(line,200,(fp))!=NULL){
			printf("%s\n", line);
		}	
		fclose(fp);
	}
	return;
}

void get_lost_log_identifier(int*identifier){
	int aux;
	char line[200];
	FILE*fp;
	fp = fopen("log.txt", "r");
	if ( fp == NULL ) {
		printf("There is no activity yet...\n");
		exit(-1);
    }else{
		while(fgets(line,200,(fp))!=NULL){
			sscanf(line, "%d", &aux);
		}	
		fclose(fp);
		(*identifier)=aux+1;
	}
	return;
}

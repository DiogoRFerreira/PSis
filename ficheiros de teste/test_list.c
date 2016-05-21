#include <stdio.h>
#include <stdlib.h>
#include "psiskv_list.h"
#include <stdint.h>

int main(){
	
	char * value[4];
    value[0]="abc";
    value[1]="csc";
    value[2]="cvc";
    value[3]="s3d";

	uint32_t key[5];
    key[0]=123;
    key[1]=124;
    key[2]=122;
    key[3]=123;
	int i=0;
	node * head = NULL;
    //char * input= (char*)malloc(128*(sizeof(char)));
    
	//printf("Add element");
	//fgets(input, 128, stdin);
	//sscanf(input, "%u %s %u %s %u %s %u %s", (unsigned)key[0], &value[0], (unsigned)key[1], &value[1], (unsigned)key[2], &value[2], (unsigned)key[3], &value[3]);
	for(i=0;i<=3;i++){
		head = add_value(head, key[i], value[i]);
	}
	for(i=0;i<=3;i++){
		head = add_value(head, key[i], value[i]);
	}
    
    print_list(head);
    
    exit(0);
}

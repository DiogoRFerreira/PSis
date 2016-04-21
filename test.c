#include <stdio.h>
#include <stdlib.h>
#include "psiskv_list.h"
#include <stdint.h>

int main(){
	
	char value1[5];
	uint32_t key[5];
	int i=0;
	node ** head = NULL;
	prinf("Add element");
	fgets(input, 128, stdin);
	sscanf(input, "%u %s %u %s %u %s %u %s", key[0], value[0], key[1], value[1], key[2], value[2], key[3], value[3]);
	for(i=0;i<3;i++){
		add_value(head, key[i], value[i]);
	}
	for(i=0;i<3;i++){
		add_value(head, key[i], value[i]);
	}
	
	
}

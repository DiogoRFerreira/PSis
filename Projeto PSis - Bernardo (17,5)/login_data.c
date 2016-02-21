#include "login_data.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structures.h"


void insert_client(variables*v,users*new_client){
	if(v->database.u.next!=NULL){
		new_client->next=v->database.u.next;
		v->database.u.next=new_client;
	}else{
		v->database.u.next=new_client;
		new_client->next=NULL;
	}
	return;
}

void remove_client(variables*v,int new_sock){
	users*aux;
	users*previous;
	
	aux=v->database.u.next;
	previous=aux;
	
	if(aux->descriptor==new_sock){/*e o primeiro*/
		v->database.u.next=aux->next;
		free(aux);
	}else{/*verificar qual dos seguintes e para apagar*/
		aux=aux->next;
		while(aux->descriptor!=new_sock){
			aux=aux->next;
			previous=previous->next;
		}
		previous->next=aux->next;
		free(aux);
	}
	return;
}

void get_username(variables v, buffer_aux *b, int new_sock){
	users*aux;
	
	aux=(v.database.u.next);
	while((aux->descriptor)!=new_sock){
		aux=(aux->next);
	}
	strcpy(b->username_sender, aux->name);
	return;
}

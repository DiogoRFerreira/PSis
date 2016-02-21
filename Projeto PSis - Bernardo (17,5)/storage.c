#include "storage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "structures.h"

void init_storage(variables*v){
	v->n_stored_messages=0;
	v->n_aux_message_storage=0;
	v->head=NULL;
	return;
}

int check_add_new_vector(variables v){
	if((v.n_stored_messages+1)>(20*v.n_aux_message_storage)){
		return 1; /*it is necessary to add a new vector to store messages*/
	}else{
		return 0;
	}
}

void add_message_vector_to_database(variables*v){
	server_database*aux;
	server_database*created=malloc(sizeof(server_database));
	created->next=NULL;
	
	if(v->n_aux_message_storage==0){
		v->head=created;
	}else{
		aux=v->head;
		while((aux->next)!=NULL){
			aux=aux->next;
		}
		aux->next=created;
	}
	v->n_aux_message_storage=(v->n_aux_message_storage)+1;
	return;
}

void get_write_position(variables v, int *index_for_list, int *index_for_vector){
	
	if((v.n_stored_messages+1)<20){
		*index_for_list=1;
		*index_for_vector=v.n_stored_messages;
	}else{
		*index_for_list=(int)ceilf(((float)(v.n_stored_messages+1))/20);
		
		if((20*(*index_for_list))!=(v.n_stored_messages+1)){
			*index_for_vector=((v.n_stored_messages+1)-(20*((*index_for_list)-1)))-1;
		}else{
			*index_for_vector=19;
		}
	}	
	return;
}

void write_position(variables*v, int index_for_list, int index_for_vector, char*m){
	server_database*aux;
	aux=v->head;
	int i=1;
	while(i!=index_for_list){
		aux=aux->next;
		i=i+1;
	}
	/*At this point the pointer is in the correct position to write
	 * the received message in the correspondent place of the array*/
	
	strcpy(aux->m_vector_20[index_for_vector].m, m);
	
	v->n_stored_messages=(v->n_stored_messages)+1;
	
	return;
}

void free_message_database(variables*v){
	server_database*aux;
	server_database*aux_next;
	
	aux=v->head;
	aux_next=v->head;
	
	if(aux==NULL){
		return;
	}
	
	if((aux->next)==NULL){//{only one message stored
		free(aux);
	}else{
		aux_next=aux_next->next;
		while(aux_next!=NULL){
			free(aux);
			aux=aux_next;
			aux_next=aux_next->next;
		}
		free(aux);
	}
	return;
}

void get_index(int query_number, int*index_for_list, int*index_for_vector){
	if(query_number<20){
		*index_for_list=1;
		*index_for_vector=query_number-1;
	}else{
		*index_for_list=(int)ceilf(((float)query_number)/20);
		
		if((20*(*index_for_list))!=query_number){
			*index_for_vector=(query_number-(20*((*index_for_list)-1)))-1;
		}else{
			*index_for_vector=19;
		}
	}
	return;
}

int check_valid_query_numbers(int total_messages, int query_number_o, int *query_number_s){
	if(((query_number_o>total_messages))||(query_number_o>(*query_number_s))){
		return 0; /* client asked for a message not stored */
	}else{
		if(((*query_number_s)>total_messages)){
			(*query_number_s)=total_messages;
		}
		return 1;
	}
}


void concatenate_query_response(variables v, int query_number_o, int query_number_s, char**query_resp){
	int index_for_list, index_for_vector;
	server_database*aux;
	aux=v.head;
	int i=1;
	char aux_[3];
	int identifier=query_number_o;
	
	if(check_valid_query_numbers(v.n_stored_messages, query_number_o, &query_number_s)==1){
		get_index(query_number_o, &index_for_list, &index_for_vector);
		
		while(i!=index_for_list){
			aux=aux->next;
			i=i+1;
		}
		/*At this point the pointer is in the correct position to get
	 * the first id number in the correspondent place of the array*/
		(*query_resp)=(char*)malloc(strlen(aux->m_vector_20[index_for_vector].m)+4);
		sprintf((*query_resp), "%d: ", query_number_o);
		strcat((*query_resp), aux->m_vector_20[index_for_vector].m);
		
		/*message of id_min successfully copied to message buffer*/
		
		for(i=1; i<=(query_number_s-query_number_o);i=i+1){
			if(i<99){
				index_for_vector=index_for_vector+1;
				if(index_for_vector==20){/*reached the end of the vector*/
					index_for_vector=0;
					/*go to next element of list*/
					aux=aux->next;
				}		
				(*query_resp)=realloc((*query_resp), strlen((*query_resp))
					+strlen(aux->m_vector_20[index_for_vector].m)+4);
				strcat((*query_resp), " ");
				identifier=identifier+1;
				sprintf(aux_, "%d: ", identifier);
				strcat((*query_resp), aux_);
				strcat((*query_resp), aux->m_vector_20[index_for_vector].m);
			}
		}
	
	}else{
		(*query_resp)=(char*)malloc(strlen("asked for messges id's correspondent to unwritten messages")+1);
		strcpy((*query_resp), "asked for messges id's correspondent to unwritten messages");
	}
	return;
}

int validadate_username(variables v, char *user_verify){
	users*aux; /*this is the first element*/
	if(v.database.u.next!=NULL){
		for(aux=v.database.u.next; aux!= NULL; aux=aux->next){
			if(strcmp(user_verify, aux->name)==0)
				return 0;
		}
		return 1;
		
	}else{
		return 1;
	}
}

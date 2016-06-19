#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include "psiskv_list.h"
#include "psiskv_server.h"//mutex

node * head = NULL;

//Print of the list - just for debug
void print_list(){

    node * current = head;
    printf("--------------------- Printing list --------------------------\n\n");
    pthread_rwlock_rdlock(&rwlock);//Lock de leitura para correr a lista
    if (current==NULL) {
        printf("Empty list\n");
    }else{
        while (current!=NULL) {
            printf("		%u				V: %s\n",current->key,current->value);
            current=current->next;
        }
    }
    pthread_rwlock_unlock(&rwlock);
    printf("\n");
    printf("--------------------- END OF Printing list -------------------\n\n");
}

// Retrieve value from list if it exists
int read_value(uint32_t key, char * p){

    //Critical Region
	pthread_rwlock_rdlock(&rwlock);
	node * current = head;
	while (current!=NULL) {
		if(current->key==key){

			//Critical Region 2
			pthread_rwlock_rdlock(&rwlock2);
			strcpy(p,current->value);
			pthread_rwlock_unlock(&rwlock2);
			pthread_rwlock_unlock(&rwlock);
			return (int)(sizeof(p));
        }
        current=current->next;
    }
    pthread_rwlock_unlock(&rwlock);
    return -2;
}

// Insert
int add_value(uint32_t key, int value_length, char * value, int overwrite) {

    node * current, * previous;
    node * new_element = (node*)malloc(sizeof(node));
	new_element->key=key;
	new_element->value_length=value_length;
	//copy value
	new_element->value = (char*)malloc(value_length*sizeof(char));
	strcpy((new_element->value),value);
    int returnvalue=0;
    int added=0;

    //Critical Region
    pthread_rwlock_rdlock(&rwlock);//Lock de leitura para correr a lista
    pthread_mutex_lock(&lock);//Só um add de cada vez
	if(head==NULL){ //Lista vazia
      pthread_rwlock_wrlock(&rwlock2);//Lock de escrita para inserir na lista
		head = new_element;
		head->next = NULL;
		pthread_rwlock_unlock(&rwlock2);
    }else if(head->next==NULL){ //Só existe um elemento na lista
        pthread_rwlock_wrlock(&rwlock2);
        if (key > head->key){
            head->next = new_element;
            new_element->next = NULL;
        }else if(key < head->key){
            new_element->next = head;
            head = new_element;
        }else{
            if(overwrite == 0)returnvalue=-2;
            else if(overwrite == 1){
                free(head->value);
                head->value = (char*)malloc(value_length*sizeof(char));
                strcpy(head->value,new_element->value);
                free(new_element->value);
                free(new_element);
            }
        }
        pthread_rwlock_unlock(&rwlock2);
	}else{ 	//Mais do que um elemento na lista
        // Initialize pointers
        current = head;
        previous = head;

		while (added!=1) {
			if(current->key == key) {		// Found element with key
                if(overwrite == 0)returnvalue=-2;
                else if(overwrite == 1){
                    pthread_rwlock_wrlock(&rwlock2);
                    free(current->value);
                    current->value = (char*)malloc(value_length*sizeof(char));
                    strcpy((current->value),(new_element->value));
                    free(new_element->value);
					free(new_element);
                    pthread_rwlock_unlock(&rwlock2);
                }
                added=1;
			}else if (key > current->key) {
				if (current->next!=NULL) {
					previous = current;
					current = current->next;
				} else {
                    pthread_rwlock_wrlock(&rwlock2);
					current->next = new_element;
					current->next->next = NULL;
					pthread_rwlock_unlock(&rwlock2);
                    added=1;
				}
            }else if(key < current->key ){
                pthread_rwlock_wrlock(&rwlock2);
                new_element->next = current;
				if (head == current) {
					head = new_element;
				}else
				previous->next=new_element;
                pthread_rwlock_unlock(&rwlock2);
				added=1;

			}
		}
	}
    pthread_mutex_unlock(&lock);
    pthread_rwlock_unlock(&rwlock);
    return returnvalue;
}

// Delete
int delete_value(uint32_t key){

    node * previous, * current = head;
    int deleted=0;
    int found=0;

    previous = current;
    //Critical Region
	pthread_rwlock_wrlock(&rwlock);
	if(head==NULL) return -1;
    while (!found) {
        if(key == (current->key)){
			found=1;
        }else{
			previous = current;
			if(current->next==NULL) return -1;
			current = current->next;
		}
    }
    if(found){
			//Critical Region 2
			pthread_rwlock_wrlock(&rwlock2);
            if ((head->next)==NULL) {
                free(head->value);
                head = NULL;
            }else if (current==head){
                head = current->next;
                free(current->value);
                free(current);
            }else{
                previous->next = current->next;
                free(current->value);
                free(current);
            }
            deleted=1;
            pthread_rwlock_unlock(&rwlock2);
	}
    pthread_rwlock_unlock(&rwlock);
    return deleted;
}

//Backup
void build_backup(FILE * fp){
    node * current;

    pthread_rwlock_rdlock(&rwlock); //Enquanto faz backup só podem ler da lista
    current=head;
    while(current!=NULL){
		//mutex para escrever no backup
		pthread_rwlock_rdlock(&rwlock2);
        fprintf(fp,"%u %u\n",current->key ,current->value_length);
        fprintf(fp,"%s\n",current->value);
        pthread_rwlock_unlock(&rwlock2);
        current=current->next;

    }

    pthread_rwlock_unlock(&rwlock);
    fclose(fp);
}

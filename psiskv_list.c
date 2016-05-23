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
    
    if (current==NULL) {
        printf("Empty list\n");
    }else{
        while (current!=NULL) {
            printf("Key: %u Value: %s\n",current->key,current->value);
            current=current->next;
        }
    }
}

// Retrieve value from list if it exists
uint32_t read_value(uint32_t key, char ** p){

    //Critical Region
	pthread_rwlock_rdlock(&rwlock);
	node * current = head;
	while (current!=NULL) {
		if(current->key==key){
			pthread_rwlock_unlock(&rwlock);
			//Critical Region 2
			pthread_rwlock_rdlock(&rwlock2);
			printf("Entrou na critical region\n");
			memcpy(*p, current->value, strlen(*p));
			pthread_rwlock_unlock(&rwlock2);
			return (uint32_t)(sizeof(current->value));
        }
        current=current->next;
    }
    pthread_rwlock_unlock(&rwlock);
    return -2;
}

// Insert
uint32_t add_value(uint32_t key, char * value, int overwrite) {

    node * current, * previous;
    node * new_element = (node*)malloc(sizeof(node));
        new_element->key=key;
        //copy value
        char * temporary = (char*)malloc(strlen(value)*sizeof(char));
        strcpy(temporary,value);
        new_element->value=temporary;
    uint32_t returnvalue=0;
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
                head->value=temporary;
            }
        }
        pthread_rwlock_unlock(&rwlock2);
	}else{ 	//Mais do que um elemento na lista
        // Initialize pointers
        current = head;
        previous = head;
        
		while (added!=1) {
			if(current->key==key) {		// Found element with key
                if(overwrite == 0)returnvalue=-2;
                else if(overwrite == 1){
                    char * temporary = (char*)malloc(strlen(value)*sizeof(char));
                    strcpy(temporary,value);
                    pthread_rwlock_wrlock(&rwlock2);
                    current->value=temporary;
                    pthread_rwlock_unlock(&rwlock2);
                }
                added=1;

			}else if (current->key < key) {
				if (current->next!=NULL) {
					previous = current;
					current = current->next;
				} else {
                    pthread_rwlock_wrlock(&rwlock2);
					current->next = new_element;
                    pthread_rwlock_unlock(&rwlock2);
					current->next->next = NULL;
                    added=1;
				}
            }else if(current->key > key){
                pthread_rwlock_wrlock(&rwlock2);
                new_element->next = current;
				if (head == current) {
					head = new_element;
				}else{
					previous->next = new_element;
                    previous->next->next=current;
				}
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
uint32_t delete_value(uint32_t key){
    
    node * previous, * current = head;
    uint32_t deleted=0;
    
    previous = current;
    //Critical Region
	pthread_rwlock_wrlock(&rwlock);
    while (current!=NULL) {
        if (key == current->key) {
			//Critical Region 2
			pthread_rwlock_wrlock(&rwlock2);
            if (current->next == NULL && previous==current) {
                head = NULL;
            }else if (previous==current){
                head = current->next;
            }else{
                previous->next = current->next;
            }
            free(current->value);
            free(current);
            current = NULL;
            deleted=1;
            pthread_rwlock_unlock(&rwlock2);
        }else{
			previous = current;
			current = current->next;
        }
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
        fprintf(fp,"%u %lu\n",current->key ,strlen(current->value));
        fprintf(fp,"%s\n",current->value); 
        current=current->next;  
    }
    pthread_rwlock_unlock(&rwlock);
    fclose(fp);
}

#include <stdio.h>
#include <stdlib.h>
#include "psiskv_list.h"
#include <stdint.h>
#include <string.h>

//Print of the list - just for debug
void print_list(node * head){
    
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
char * read_value(node * head, uint32_t key){

    node * current = head;

    if (current==NULL) {
        printf("Empty list => Not on the list\n");
    }else{
		while (current!=NULL) {
			if(current->key==key){
                return current->value;
            }
            current=current->next;
        }
    }
    
    return NULL;
}

// Insert
uint32_t add_value(node ** head, uint32_t key, char * value, int overwrite) {

    node * current, * previous;
    node * new_element = (node*)malloc(sizeof(node));
        new_element->key=key;
        new_element->value=value;
    uint32_t returnvalue=0;
    int added=0;

	if((*head)==NULL){ //Lista vazia
		(*head) = new_element;
		(*head)->next = NULL;
    }else if((*head)->next==NULL){ //Só existe um elemento na lista
        if (key > (*head)->key){
            (*head)->next = new_element;
            new_element->next = NULL;
        }else if(key < (*head)->key){
            new_element->next = *head;
            *head = new_element;
        }else{//(key == (*head)->key)
            //char * temporary = (char*)malloc((strlen(value)+strlen((*head)->value))*sizeof(char));
            //sprintf(temporary, "%s%s", (*head)->value, value);
            //(*head)->value = temporary;
            if(overwrite == 0)returnvalue=-2;
            else if(overwrite == 1){
                char * temporary = (char*)malloc(strlen(value)*sizeof(char));
                strcpy(temporary,value);
                (*head)->value=temporary;
            }
        }
	}else{ 	//Mais do que um elemento na lista
        // Initialize pointers
        current = *head;
        previous = *head;
        
		while (added!=1) {
			if(current->key==key) {		// Found element with key
				//char * temporary = (char*)malloc((strlen(value)+strlen(current->value))*sizeof(char));
				//sprintf(temporary, "%s%s", current->value, value);
                //current->value = temporary;//
                if(overwrite == 0)returnvalue=-2;
                else if(overwrite == 1){
                    char * temporary = (char*)malloc(strlen(value)*sizeof(char));
                    strcpy(temporary,value);
                    current->value=temporary;
                }
                added=1;

			}else if (current->key < key) {
				if (current->next!=NULL) {
					previous = current;
					current = current->next;
				} else {
					current->next = new_element;
					current->next->next = NULL;
                    added=1;
				}
            }else if(current->key > key){
                new_element->next = current;
				if (*head == current) {
					*head = new_element;
				}else{
					previous->next = new_element;
                    previous->next->next=current;
				}
				added=1;
			}
		}
	}
    return returnvalue;
}

// Delete
uint32_t delete_value(node ** head, uint32_t key){
    
    node * previous, * current = *head;
    uint32_t deleted=0;
    
    previous = current;
    while (current!=NULL) {
        if (key == current->key) {
            if (current->next == NULL && previous==current) {
                *head = NULL;
            }else if (previous==current){
                *head = current->next;
            }else{
                previous->next = current->next;
            }
            free(current);
            current = NULL;
            deleted=1;
        }else{
			previous = current;
			current = current->next;
        }
    }
    return deleted;
}


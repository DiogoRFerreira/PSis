#include <stdio.h>
#include <stdlib.h>
#include "psiskv_list.h"
#include <stdint.h>
#include <string.h>

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



//Insert
void add_value(node ** head, uint32_t key, char * value) {


	node * current, *previous, * new_element = (node*)malloc(sizeof(node));
	new_element->key=key;
	new_element->value=value;

   
	// Initialize pointers
	current = *head;
	previous = *head;
	
	if (*head==NULL) {	// Check if the list is empty.
		*head = new_element;
		new_element->next = NULL;
		// (*num_elements_ptr)++;
		return;
	}else{ 							// In case it is not empty.
		while (current!=NULL) {
			if(current->key==key) {		// Found element with key
				char * temporary = (char*)malloc((strlen(value)+strlen(current->value))*sizeof(char));
				sprintf(temporary, "%s%s", current->value, value);
				return;
			}else if ((current->next == NULL) && (previous == (current))){      // Check if current is the last AND the only element on the list.
				 if (key > current->key){       						 // Determines if the new element should be placed before or
					current->next = new_element;                        // after the current element according to its name.
					new_element->next = NULL;
				 }else{
					new_element->next = current;
					*head = new_element;                        // If it is inserted before, the new head pointer of the list
				 }                                                      // is now the pointer to the new element.
				//(*num_elements_ptr)++;
				return;
			}else if (key > current->key) {
				previous = current;
				if (current->next!=NULL) {                      // If it is NOT the last on the list then continues.
					previous = current;
					current = current->next;
				} else {
					current->next = new_element;
					new_element->next = NULL;
				}
            }else{                                      // If the new name is smaller than the current name, it adds it before.
				new_element->next = current;
				if (*head == current) {
					*head = new_element;
				}else{
					previous->next = new_element;
				}
				//(*num_elements_ptr)++;
			}
		}
	}
}

// Delete: 0 -> Node not found; 1 -> Node deleted
int delete_value(node ** head, uint32_t key){
    node * previous, *current = *head;
    previous = current;
    while (current!=NULL) {
        if (key == current->key) {
            if (current->next==NULL && previous==current) {
                *head = NULL;
            }else if (previous==current){
                    *head = current->next;
                }else {
                    previous->next = current->next;
                }
            //(*num_elements_ptr)--;
            free(current);
            return 1;
        }else{
			previous = current;
			current = current->next;
        }
    }
    return 0;
}


/* Aidan Trent
 * A linked list.
 * Limited to 65,535 KB nodes, easy as changing some data types if an increase
 * is needed though.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "linkedList.h"

// Do not define in header, only used by other saveManager functions
Node* makeNode(Entity* data){
	Node* newNode = malloc(sizeof(Node*) + sizeof(Entity));
	if (newNode == NULL){
		fprintf(stderr, "ERROR: malloc fail for newNode @ makeNode\n");
		return(NULL);
	}

	newNode->next = NULL;
	newNode->ent.frameD = data->frameD;
	newNode->ent.frameR = data->frameR;
	return(newNode);
}

// Make a list containing one node with given params
LList* makeList(Entity* data){
	LList* newList;
	newList = malloc(sizeof(Node*) * 2);
	if (newList == NULL){
		fprintf(stderr, "ERROR: malloc fail for newList @ makeList\n");
		return(NULL);
	}
	Node* newNode = makeNode(data);
	if (newNode == NULL){
		return(NULL);
	}

	newList->head = newNode;
	newList->tail = newNode;

	return(newList);
}

// Insert a Node of given params before the head of list
Node* insertHead(LList* list, Entity* data){
	Node* newNode = makeNode(data);
	if (newNode == NULL){
		return(NULL);
	}

	if (list->head == NULL){
		list->head = newNode;
		list->tail = newNode;
		return(newNode);
	}

	newNode->next = list->head;
	list->head = newNode;
	return(newNode);
}

// Insert a Node of given params after the tail of list
Node* insertTail(LList* list, Entity* data){
	Node* newNode = makeNode(data);
	if (newNode == NULL){
		return(NULL);
	}

	if (list->tail == NULL){
		list->head = newNode;
		list->tail = newNode;
		return(newNode);
	}

	list->tail->next = newNode;
	list->tail = newNode;
	return(newNode);
}

void freeList(LList* list){
	Node* temp;

	while (list->head != NULL){
		temp = list->head;
		list->head = list->head->next;
		free(temp);
	}
}

// Remove node from list when previous node is known.
// Returns 1 when nothing is freed, 0 on success
int freeNodeFast(LList* list, Node* prevNode, Node* delNode){
	if (delNode == list->head){
		if (list->head->next == NULL){
			list->head = NULL;
			list->tail = NULL;
			free(delNode);
			return(0);
		}
		else{
			list->head = list->head->next;
			free(delNode);
			return(0);
		}
	}
	else if (delNode == list->tail){
		if (prevNode->next != delNode){
			fprintf(stderr, "ERROR: prevNode is not previous to delNode @ freeNodeFast\n");
			return(1);
		}
		else{
			list->tail = prevNode;
			prevNode->next = NULL;
			free(delNode);
			return(0);
		}
	}
	prevNode->next = prevNode->next->next;
	free(delNode);
	return(0);
}

// Remove node from list.
// Returns 1 when nothing is freed, 0 on success
int freeNode(LList* list, Node* delNode){
	if (delNode == list->head){
		if (list->head->next == NULL){
			list->head = NULL;
			list->tail = NULL;
			free(delNode);
			return(0);
		}
		else{
			list->head = list->head->next;
			free(delNode);
			return(0);
		}
	}
	if (list->head->next == NULL){
		fprintf(stderr, "ERROR: delNode does note exist in list @ freeNode\n");
		return(1);
	}
	Node* curNode = list->head->next;
	Node* prevNode = list->head;
	do{
		if (curNode == delNode){
			if (delNode == list->tail){
				list->tail = prevNode;
				prevNode->next = NULL;
				free(delNode);
				return(0);
			}
			else{
				prevNode->next = prevNode->next->next;
				free(delNode);
				return(0);
			}
		}
		prevNode = curNode;
		curNode = curNode->next;
	} while (curNode != NULL);
	return(1);
}

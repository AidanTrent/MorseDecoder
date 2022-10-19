#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <stdint.h>

typedef struct{
	uint64_t frameD; // frame depressed
	uint64_t frameR; // frame released
} Entity;

typedef struct Node{
	struct Node* next;
	Entity ent;
} Node;

typedef struct{
	Node* head;
	Node* tail;
} LList;

// Make a list containing one node with given params
LList* makeList(Entity* data);

// Insert a Node of given params before the head of list
Node* insertHead(LList* list, Entity* data);

// Insert a Node of given params after the tail of list
Node* insertTail(LList* list, Entity* data);

void freeList(LList* list);

// Remove node from list when previous node is known.
// Returns 1 when nothing is freed, 0 on success
int freeNodeFast(LList* list, Node* prevNode, Node* delNode);

// Remove node from list.
// Returns 1 when nothing is freed, 0 on success
int freeNode(LList* list, Node* node);
#endif


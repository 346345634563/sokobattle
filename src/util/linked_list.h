#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdlib.h>

typedef struct Node{
    void* val;
    struct Node* next;
}Node;

typedef struct Linked_list{
    Node* head;
    Node* tail;
    Node* current;
    int size;
}Linked_list;


void Node_Init(Node* n, void* val);

void Linked_List_Init(Linked_List* l);
void Linked_List_Append(Linked_List* l, void* val);
void* Linked_List_Pop(Linked_List* l);
void Linked_List_Clean(Linked_List* l);

#endif

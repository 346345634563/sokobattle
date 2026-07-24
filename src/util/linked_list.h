#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "gameplay/board.h"

#include <stdlib.h>


typedef struct Node{
    Move val;
    struct Node* next;
}Node;

typedef struct Linked_List{
    Node* head;
    Node* tail;
    Node* current;
    int size;
}Linked_List;


void Node_Init(Node* n, Move val);

void Linked_List_Init(Linked_List* l);
void Linked_List_Append(Linked_List* l, Move val);
Move Linked_List_Pop(Linked_List* l);
void Linked_List_Clean(Linked_List* l);
void Linked_List_Reverse(Linked_List* l);
void Linked_List_Print(Linked_List* l);

#endif

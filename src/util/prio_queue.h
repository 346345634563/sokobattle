#ifndef PRIO_QUEUE_H_
#define PRIO_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include "gameplay/board.h"

typedef struct Node_Prio{
    int g;
    int h;
    Move m;                  // move made to get there
    Board_State* board_state;
    struct Node_Prio* parent;
    struct Node_Prio* next;
}Node_Prio;

typedef struct Prio_Queue{
    Node_Prio* head;
    int size;
}Prio_Queue;


void Prio_Queue_Init(Prio_Queue* pq);
void Prio_Queue_Clean(Prio_Queue* pq);
void Prio_Queue_Add(Prio_Queue* pq, Node_Prio* parent, Board_State* b, int g, int h, Move m);
Node_Prio* Prio_Queue_Pop(Prio_Queue* pq);



#endif

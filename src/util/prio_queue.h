#ifndef PRIO_QUEUE_H_
#define PRIO_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include "core/board.h"

typedef struct Node_Prio{
    int g;
    int h;
    Board_State* board_state;
    struct Node_Prio* next;
}Node_Prio;

typedef struct Prio_Queue{
    Node* head;
    int size;
}


void Prio_Queue_Init(Prio_Queue* pq);
void Prio_Queue_Clean(Prio_Queue* pq);
void Prio_Queue_Add(Board_State* b, int g, int h);
void Prio_Queue_Pop(Prio_Queue* pq);



#endif

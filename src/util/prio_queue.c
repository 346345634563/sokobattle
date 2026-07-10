#include "prio_queue.h"
#include <stdlib.h>

void Prio_Queue_Init(Prio_Queue* pq) {
    pq->head = NULL;
    pq->size = 0;
}

void Prio_Queue_Clean(Prio_Queue* pq) {
    Node_Prio* curr = pq->head;
    while (curr) {
        Node_Prio* next = curr->next;
        Free_Board_State(pq->curr->board_state);
        free(curr);
        curr = next;
    }
    pq->head = NULL;
    pq->size = 0;
}

void Prio_Queue_Add(Prio_Queue* pq, Board_State* b, int g, int h) {
    Node_Prio* new_node = malloc(sizeof(Node_Prio));
    new_node->board_state = b;
    new_node->g = g;
    new_node->h = h;
    new_node->next = NULL;

    if (pq->head == NULL || h < pq->head->h || (h == pq->head->h && g < pq->head->g)) {
        new_node->next = pq->head;
        pq->head = new_node;
        pq->size++;
        return;
    }

    Node_Prio* curr = pq->head;
    while (curr->next != NULL &&
           (curr->next->h < h ||
            (curr->next->h == h && curr->next->g < g))) {
        curr = curr->next;
    }
    
    new_node->next = curr->next;
    curr->next = new_node;
    pq->size++;
}

Node_Prio* Prio_Queue_Pop(Prio_Queue* pq) {
    if (pq->head == NULL) return NULL;
    Node_Prio* old_head = pq->head;
    pq->head = old_head->next;
    pq->size--;
    return old_head;
}

Node_Prio* Prio_Queue_Peek(Prio_Queue* pq) {
    return pq->head;
}

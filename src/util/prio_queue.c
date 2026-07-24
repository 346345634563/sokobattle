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
        Free_Board_State(curr->board_state);
        free(curr);
        curr = next;
    }
    pq->head = NULL;
    pq->size = 0;
}

void Prio_Queue_Add(Prio_Queue* pq, Node_Prio* parent, Board_State* b, int g, int h, Move m) {
    Node_Prio* new_node = malloc(sizeof(Node_Prio));
    new_node->parent = parent;
    new_node->board_state = b;
    new_node->g = g;
    new_node->h = h;
    new_node->m = m;
    new_node->next = NULL;

    int new_f = g + h;

    if (pq->head == NULL || new_f < (pq->head->g + pq->head->h)) {
        new_node->next = pq->head;
        pq->head = new_node;
        pq->size++;
        return;
    }

    Node_Prio* curr = pq->head;
    while (curr->next != NULL &&
           (curr->next->g + curr->next->h) < new_f) {
        curr = curr->next;
    }

    new_node->next = curr->next;
    curr->next = new_node;
    pq->size++;
  //  log_info("Prioqueue size : %d", pq->size);
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

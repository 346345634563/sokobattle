#include "linked_list.h"

void Linked_List_Init(Linked_List* l) {
    l->head = NULL;
    l->tail = NULL;
    l->current = NULL;
    l->size = 0;
}

void Linked_List_Append(Linked_List* l, Move val) {
    Node* new_node = malloc(sizeof(Node));
    if (!new_node) return;
    new_node->val = val;
    new_node->next = NULL;

    if (l->size == 0) {
        l->head = new_node;
        l->tail = new_node;
    } else {
        l->tail->next = new_node;
        l->tail = new_node;
    }
    l->size++;
}

Move Linked_List_Pop(Linked_List* l) {
    if (l->size == 0) return -1;  
    Move val;
    if (l->size == 1) {
        val = l->head->val;
        free(l->head);
        l->head = NULL;
        l->tail = NULL;
    } else {
        Node* current = l->head;
        while (current->next != l->tail) {
            current = current->next;
        }
        val = l->tail->val;
        free(l->tail);
        l->tail = current;
        l->tail->next = NULL;
    }
    l->size--;
    return val;
}

void Linked_List_Clean(Linked_List* l) {
    Node* current = l->head;
    while (current) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    l->head = NULL;
    l->tail = NULL;
    l->current = NULL;
    l->size = 0;
}

void Linked_List_Reverse(Linked_List* l) {
    Node* prev = NULL;
    Node* curr = l->head;
    Node* next = NULL;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    l->tail = l->head;   
    l->head = prev;      
}

void Linked_List_Print(Linked_List* l) {
    if (!l) return;
    const char* move_names[] = {"N", "E", "S", "W"};
    Node* curr = l->head;
    while (curr) {
        printf("%s ", move_names[curr->val]);
        curr = curr->next;
    }
    printf("\n");
}

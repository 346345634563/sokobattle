#include "hashset.h"

#define LOAD_FACTOR 0.75  

void Hashset_Init(Hashset* h) {
    h->capacity = 2 << 16;  
    h->size = 0;
    h->buckets = (Node_Hashset**)malloc(h->capacity * sizeof(Node_Hashset*));
    for (int i = 0; i < h->capacity; i++) {
        h->buckets[i] = NULL;
    }
}

// djb2 hash
uint32_t Hashset_Hash(Board_State* b) {
    uint32_t hash = 5381;
    hash = ((hash << 5) + hash) + b->p1.x;
    hash = ((hash << 5) + hash) + b->p1.y;
    // Boxes
    for (int i = 0; i < b->num_boxes; i++) {
        hash = ((hash << 5) + hash) + b->boxes[i].x;
        hash = ((hash << 5) + hash) + b->boxes[i].y;
    }
    // Goals
    for (int i = 0; i < b->num_goals; i++) {
        hash = ((hash << 5) + hash) + b->goals[i].x;
        hash = ((hash << 5) + hash) + b->goals[i].y;
    }
    return hash;
}

bool Hashset_Equals(Board_State* a, Board_State* b) {
    if (a->p1.x != b->p1.x || a->p1.y != b->p1.y){
        return false;
    }
    for (int i = 0; i < a->num_boxes; i++) {
        if (a->boxes[i].x != b->boxes[i].x || a->boxes[i].y != b->boxes[i].y)
            return false;
    }
    for (int i = 0; i < a->num_goals; i++) {
        if (a->goals[i].x != b->goals[i].x || a->goals[i].y != b->goals[i].y)
            return false;
    }
    return true;
}

void Hashset_Resize(Hashset* h, int new_capacity) {
    log_info("Hashset resized from %d to %d", h->capacity, new_capacity); 
    Node_Hashset** old_buckets = h->buckets;
    int old_capacity = h->capacity;

    h->buckets = (Node_Hashset**)calloc(new_capacity, sizeof(Node_Hashset*));
    h->capacity = new_capacity;
    h->size = 0;

    for (int i = 0; i < old_capacity; i++) {
        Node_Hashset* current = old_buckets[i];
        while (current != NULL) {
            Node_Hashset* next = current->next;
            uint32_t hash = Hashset_Hash(current->board_state);
            int index = hash % h->capacity;
            current->next = h->buckets[index];
            h->buckets[index] = current;
            h->size++;
            current = next;
        }
    }
    free(old_buckets);
}

void Hashset_Add(Hashset* h, Board_State* b, int cost) {
    if ((float)h->size / h->capacity > LOAD_FACTOR) {
        Hashset_Resize(h, h->capacity * 2);
    }

    uint32_t hash = Hashset_Hash(b);
    int index = hash % h->capacity;

    Node_Hashset* current = h->buckets[index];
    while (current != NULL) {
        if (Hashset_Equals(current->board_state, b)) {
            if (cost < current->best_g) {
                current->best_g = cost;
            }
            return;
        }
        current = current->next;
    }

    Node_Hashset* new_node = malloc(sizeof(Node_Hashset));
    new_node->board_state = b;
    new_node->best_g = cost;
    new_node->next = h->buckets[index];
    h->buckets[index] = new_node;
    h->size++;
}



int Hashset_Get_Min_Cost(Hashset* h, Board_State* b) {
    if (h->buckets == NULL || h->size == 0){
        return -1;
    }

    uint32_t hash = Hashset_Hash(b);
    int index = hash % h->capacity;

    Node_Hashset* current = h->buckets[index];
    while (current != NULL) {
        if (Hashset_Equals(current->board_state, b)){
            return current->best_g;
        }
        current = current->next;
    }
    return -1;
}

bool Hashset_Contains(Hashset* h, Board_State* b) {
    return Hashset_Get_Min_Cost(h, b) != -1;
}




void Hashset_Clean(Hashset* h) {
    if (h->buckets == NULL) return;
    for (int i = 0; i < h->capacity; i++) {
        Node_Hashset* current = h->buckets[i];
        while (current != NULL) {
            Node_Hashset* next = current->next;
            free(current);
            current = next;
        }
    }
    free(h->buckets);
    h->buckets = NULL;
    h->capacity = 0;
    h->size = 0;
}

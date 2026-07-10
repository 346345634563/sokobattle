#include "hashset.h"

void Hashset_Init(Hashset* h){
    h->capacity = 2 << 16;
    h->size = 0;
    h->buckets = (Node_Hashset**) malloc(h->capacity * sizeof(Node_Hashset*));
}



// djb2 hash
uint32_t Hashset_Hash(Board_State b){
    
    uint32_t magic_num = 5381;  
    uint32_t hash = 0;    
    
    for(int i = 0; i < b.num_boxes; i++){
        hash = ((hash << 5) + hash) + b.goals[i].x + b.goals[j] + b.boxes[i] + b.boxes[j];
    }
    
    return hash;
}

bool Hashset_Equals(Board_State* a, Board_State* b) {
    
    for (int i = 0; i < a->num_boxes; i++) {
        if (a->boxes[i].x != b->boxes[i].x || a->boxes[i].y != b->boxes[i].y  || a->goals[i].x != b->goals[i].x ||a->goals[i].y != b->goals[i].y){
            return false;
        }
    }
    
    if (a->player.x != b->player.x || a->player.y != b->player.y){
        return false;
    }

    return true; 
}


void Hashset_Resize(Hashset* h) {
    
    Node_Hashset** old_buckets = h->buckets;
    int old_capacity = h->capacity;

    h->buckets = (Node_Hashset**)calloc(new_capacity, sizeof(Node_Hashset*));


    h->capacity = old_capacity << 2;
    h->size = 0;

    for (int i = 0; i < old_capacity; i++) {
        
        Node_Hashset* current = old_buckets[i];
        
        while (current != NULL) {
            Node_Hashset* next = current->next;

            uint32_t hash = Hashset_Hash(current->board_state);
            int index = hash % new_capacity;

            current->next = h->buckets[index];
            h->buckets[index] = current;
            h->size++;

            current = next;
        }
    }

    free(old_buckets);
}

void Hashset_Add(Hashset* h, Board_State* b) {

    
    if ((float)h->size / h->capacity > LOAD_FACTOR) {
        Hashset_Resize(h, h->capacity * 2);
    }
    
    uint32_t hash = Hashset_Hash(b);
    int index = hash % h->capacity;
    
    Node_Hashset* current = h->buckets[index];
    while (current != NULL) {
        if (Hashset_Equals(current->board_state, b)) {
            return;         }
        current = current->next;
    }
    
    Node_Hashset* new_node = (Node_Hashset*)malloc(sizeof(Node_Hashset));

    
    new_node->board_state = b;
    new_node->next = h->buckets[index];
    h->buckets[index] = new_node;
    h->size++;
}

bool Hashset_Contains(Hashset* h, Board_State* b) {
    if (h->buckets == NULL || h->size == 0){
        return false;
    }
    
    uint32_t hash = Hashset_Hash(b);
    int index = hash % h->capacity;
    
    Node_Hashset* current = h->buckets[index];
    while (current != NULL) {
        if (Hashset_Equals(current->board_state, b)) {
            return true;
        }
        current = current->next;
    }
    
    return false;
}






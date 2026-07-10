#ifndef HASHSET_H_
#define HASHSET_H_

#include "core/board.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node_Hashset {
    Board_State* board_state;
    struct Node_Hashset* next;
} Node_Hashset;

typedef struct Hashset {
    Node_Hashset** buckets;  
    int capacity;
    int size;                
} Hashset;

void Hashset_Init(Hashset* h);
void Hashset_Clean(Hashset* h);
uint32_t Hashset_Hash(Board_State* b);
bool Hashset_Equals(Board_State* a, Board_State* b);
void Hashset_Add(Hashset* h, Board_State* b);
bool Hashset_Contains(Hashset* h, Board_State* b);
void Hashset_Resize(Hashset* h, int new_capacity);

#endif

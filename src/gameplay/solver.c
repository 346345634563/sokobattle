#include "solver.h"







Board_State* Make_Move_Solver(Map map, Board_State* a, Move move){
    
    Board_State* b = Copy_Board_State(a);

    int dx = 0, dy = 0;
    switch(move){
        case MOVE_UP:    dx =  0; dy = -1; break;
        case MOVE_DOWN:  dx =  0; dy =  1; break;
        case MOVE_LEFT:  dx = -1; dy =  0; break;
        case MOVE_RIGHT: dx =  1; dy =  0; break;
        default: return NULL;
    }

    uint8_t x = b->p1.x;
    uint8_t y = b->p1.y;

    int tx = x + dx; 
    int ty = y + dy; 

    if (tx < 0 || tx >= map.width || ty < 0 || ty >= map.height){
        return NULL;
    }

    if(map.tiles[ty][tx] == TILE_WALL){
        return NULL;
    }

    int box_idx = -1;
    for(int i = 0; i < b->num_boxes; i++){
        if(b->boxes[i].x == tx && b->boxes[i].y == ty){
            box_idx = i;
            break;
        }
    }

    if(box_idx >= 0){
        int bx2 = tx + dx; 
        int by2 = ty + dy; 

        if(bx2 < 0 || bx2 >= map.width || by2 < 0 || by2 >= map.height){
            return NULL;
        }

        if(map.tiles[by2][bx2] == TILE_WALL){
            return NULL;
        }

        for(int i = 0; i < b->num_boxes; i++){
            if(i == box_idx){
                continue;
            }

            if(b->boxes[i].x == bx2 && b->boxes[i].y == by2){
                return NULL;
            }

        }

        b->boxes[box_idx].x = bx2;
        b->boxes[box_idx].y = by2;
    }

    b->p1.x = tx;
    b->p1.y = ty;

    return b;
}


Linked_List* solve(const Map m, Board_State b) {
    log_info("Attempting to solve");

    Board_State* init_copy = Copy_Board_State(&b);
    if (!init_copy){
        return NULL;
    }

    Prio_Queue pq;
    Prio_Queue_Init(&pq);
    Prio_Queue_Add(&pq, NULL, init_copy, 0, heuristic(init_copy), MOVE_COUNT);

    Hashset visited;
    Hashset_Init(&visited);
    Hashset_Add(&visited, init_copy, 0);

    Node_Prio* current;
    Node_Prio* goal_node = NULL;

    while (pq.size > 0) {
        current = Prio_Queue_Pop(&pq);

        int best_g = Hashset_Get_Min_Cost(&visited, current->board_state);
        if (best_g != -1 && current->g > best_g) {
            Free_Board_State(current->board_state);
            free(current);
            continue;
        }

        if (Is_Game_Over(*current->board_state)) {
            goal_node = current;
            break;
        }

        for (int i = 0; i < MOVE_COUNT; i++) {
            Move move = (Move)i;
            Board_State* new_state = Make_Move_Solver(m, current->board_state, move);
            if (!new_state){
                continue;
            }

            int new_g = current->g + 1;
            int new_h = heuristic(new_state);

            int old_g = Hashset_Get_Min_Cost(&visited, new_state);
            if (old_g != -1 && old_g <= new_g) {
                Free_Board_State(new_state);
                free(new_state);
                continue;
            }

            Prio_Queue_Add(&pq, current, new_state, new_g, new_h, move);
            Hashset_Add(&visited, new_state, new_g);   
        }

    }

    Linked_List* solution = malloc(sizeof(Linked_List));
    if (!solution) {
        Prio_Queue_Clean(&pq);
        Hashset_Clean(&visited);
        return NULL;
    }
    Linked_List_Init(solution);

    if (goal_node) {
        Node_Prio* cur = goal_node;
        while (cur->parent != NULL) {
            Linked_List_Append(solution, cur->m);
            cur = cur->parent;
        }
    }

    Prio_Queue_Clean(&pq);
    Hashset_Clean(&visited);
    Linked_List_Print(solution);
    return solution;
}



int manhattan(int x1, int y1, int x2, int y2){
    int x = (x1 > x2) ? x1 - x2 : x2 - x1;
    int y = (y1 > y2) ? y1 - y2 : y2 - y1;
    return x + y;
}

// to have a optimal heuristic, we must always overestimate the distance
int heuristic(const Board_State* b) {
    int h = 0;
    for (int i = 0; i < b->num_boxes; i++) {
        int min_dist = INT_MAX;
        for (int j = 0; j < b->num_goals; j++) {
            int dist = manhattan(b->boxes[i].x, b->boxes[i].y,
                                 b->goals[j].x, b->goals[j].y);
            if (dist < min_dist) min_dist = dist;
        }
        if (min_dist != INT_MAX) h += min_dist;
    }
    return h;
}



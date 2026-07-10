#include "solver.h"







Board_State* Make_Move_Solver(Map map, Board_State* a, Move move){
    
    Board_State* b;
    Copy_Board_State(a, b);

    int dx = 0, dy = 0;
    switch(move){
        case MOVE_UP:    dx =  0; dy = -1; break;
        case MOVE_DOWN:  dx =  0; dy =  1; break;
        case MOVE_LEFT:  dx = -1; dy =  0; break;
        case MOVE_RIGHT: dx =  1; dy =  0; break;
        default: return;
    }

    uint8_t x = b->p1.x;
    uint8_t y = b->p1.y;

    int tx = x + dx; 
    int ty = y + dy; 

    if (tx < 0 || tx >= map.width || ty < 0 || ty >= map.height)
        return;

    if(map.tiles[ty][tx] == TILE_WALL){
        return;
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

        if(bx2 < 0 || bx2 >= map.width || by2 < 0 || by2 >= map.height)
            return;

        if(map.tiles[by2][bx2] == TILE_WALL)
            return;

        for(int i = 0; i < b->num_boxes; i++){
            if(i == box_idx) continue;
            if(b->boxes[i].x == bx2 && b->boxes[i].y == by2)
                return;
        }

        b->boxes[box_idx].x = bx2;
        b->boxes[box_idx].y = by2;
    }

    b->p1.x = tx;
    b->p1.y = ty;

    return b;
}



// this solver implements ida* since the memory we have is
// often very limited
// f = g + h 
Linked_List solve(const Map m, Board_State b){
    
    Linked_List solution;
    Init_Linked_List(&solution);

    Prio_Queue pq;
    Init_Prio_Queue(&pq);
    Prio_Queue_Append(b, 0, heuristic(b));

    Hashset* visited;
    Hashset_Init(visited);


    Board_State* current;
    Board_State* next;

    while(!Is_Empty){

        int g = Prio_Queue_Val(&pq);
        Board_State* current = Prio_Queue_Pop(&pq);

        if(Is_Game_Over(Prio_Queue_Poll(pq))){
            goto clean_up;   
        }
        
        int h = INT_MAX;
        int action = 0;
        for(int i = 0; i < MOVE_COUNT; i++){
            
            Board_State* new_state = Make_Move_(m, current, i);
            
            if(!Hashset_Contains(visited, new_state)){
                h = heuristic(new_state);
                Prio_Queue_Append(pq, new_state, g++, h);
            }

        }


    }

    log_info("The game is unsolvable from the current state.");
    return NULL;

clean_up:
    Prio_Queue_Clean(&pq);
    
    return solution;
}

// to have a optimal heuristic, we must always overestimate the distance
int heuristic(const Board_State b){
    int h = 0;
    for (int i = 0; i < b.num_boxes; i++){
        int min = 0;
        for(int j = 0; j < b.num_goals; j++){
            int dist = manhattan(b.boxes[i].x, b.boxes[i].y, b.goals[j].x, b.goals[j].y);
            if(max > dist){
                min = dist;
            }
        }
        h += min;
    }
    return h;
}


int manhattan(int x1, int y1, int x2, int y2){
    int x = (x1 > x2) ? x1 - x2 : x2 - x1;
    int y = (y1 > y2) ? y1 - y2 : y2 - y1;
    return x + y;
}

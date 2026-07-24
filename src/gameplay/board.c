#include "board.h"

#define MAX_LINES 256


void Load_Level(Board* b, const char* filename) {
    char* file_data = LoadFileText(filename);
    if (!file_data) {
        log_fatal("Cannot load level file: %s", filename);
    }

    char* lines[256]; 
    int line_count = 0;
    char* line = strtok(file_data, "\n");
    while (line != NULL && line_count < 256) {
        if (strlen(line) > 0 && line[0] != ';') {   
            lines[line_count++] = line;
        }
        line = strtok(NULL, "\n");
    }

    if (line_count == 0) {
        log_fatal("No valid lines in level file.");
    }

    int width = 0;
    for (int i = 0; i < line_count; i++) {
        int len = strlen(lines[i]);
        if (len > width) width = len;
    }
    int height = line_count;

    b->map.width = width;
    b->map.height = height;
    b->map.tiles = (uint8_t**)malloc(height * sizeof(uint8_t*));
    for (int y = 0; y < height; y++) {
        b->map.tiles[y] = (uint8_t*)malloc(width * sizeof(uint8_t));
        // Par défaut : vide
        for (int x = 0; x < width; x++) {
            b->map.tiles[y][x] = TILE_EMPTY;
        }
    }

    int num_boxes = 0, num_goals = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char c = (x < strlen(lines[y])) ? lines[y][x] : ' ';
            if (c == '$' || c == '*') num_boxes++;
            if (c == '.' || c == '*' || c == '+') num_goals++;
        }
    }

    b->board_state.num_boxes = 0;
    b->board_state.num_goals = 0;
    b->board_state.boxes = malloc(num_boxes * sizeof(Box));
    b->board_state.goals = malloc(num_goals * sizeof(Goal));
    if (!b->board_state.boxes || !b->board_state.goals) {
        log_fatal("Memory allocation failed for boxes/goals.");
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char c = (x < strlen(lines[y])) ? lines[y][x] : ' ';
            // Mur
            if (c == '#') {
                b->map.tiles[y][x] = TILE_WALL;
            }
            // Joueur
            else if (c == '@' || c == '+') {
                b->board_state.p1.x = x;
                b->board_state.p1.y = y;
                if (c == '+') {
                    b->map.tiles[y][x] = TILE_EMPTY;  
                }
            }
            else if (c == '$' || c == '*') {
                int idx = b->board_state.num_boxes++;
                b->board_state.boxes[idx].x = x;
                b->board_state.boxes[idx].y = y;
            }
            if (c == '.' || c == '*' || c == '+') {
                int idx = b->board_state.num_goals++;
                b->board_state.goals[idx].x = x;
                b->board_state.goals[idx].y = y;
            }
        }
    }
    
    b->initial_board_state.boxes = malloc(b->board_state.num_boxes * sizeof(Box));
    b->initial_board_state.goals = malloc(b->board_state.num_goals * sizeof(Goal));
    if (!b->initial_board_state.boxes || !b->initial_board_state.goals) {
        log_fatal("Memory allocation failed for initial state");
    }
    Copy_Board_State_Reset(&b->board_state, &b->initial_board_state);

    log_info("Level loaded: width=%d, height=%d, boxes=%d, goals=%d",
             width, height, b->board_state.num_boxes, b->board_state.num_goals);

    UnloadFileText(file_data);
}



void Make_Move(Map map, Board_State* b, Move move){
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
}



bool Is_Game_Over(Board_State b){

    for(int i = 0; i < b.num_boxes; i++){
        bool on_goal = false;
        for(int j = 0; j < b.num_goals; j++){
            if(b.boxes[i].x == b.goals[j].x && b.boxes[i].y == b.goals[j].y){
                on_goal = true;
                break;
            }
        }
        if(!on_goal){
            return false;
        }
    }
    return true;
}

void Free_Board_State(Board_State* b){
    free(b->boxes);
    free(b->goals);
    b->boxes = NULL;
    b->goals = NULL;
    b->num_boxes = 0;
    b->num_goals = 0;
}

void Free_Board(Board* b) {
    if (b->map.tiles) {
        for (int i = 0; i < b->map.height; i++) {
            free(b->map.tiles[i]);
        }
        free(b->map.tiles);
        b->map.tiles = NULL;
    }
    Free_Board_State(&b->board_state);
}


Board_State* Copy_Board_State(const Board_State* src) {
    Board_State* dst = malloc(sizeof(Board_State));
    if (!dst) return NULL;

    dst->num_boxes = src->num_boxes;
    dst->num_goals = src->num_goals;
    dst->p1 = src->p1;

    dst->boxes = malloc(src->num_boxes * sizeof(Box));
    dst->goals = malloc(src->num_goals * sizeof(Goal));

    if (!dst->boxes || !dst->goals) {
        free(dst->boxes);
        free(dst->goals);
        free(dst);
        return NULL;
    }

    for (int i = 0; i < src->num_boxes; i++)
        dst->boxes[i] = src->boxes[i];
    for (int i = 0; i < src->num_goals; i++)
        dst->goals[i] = src->goals[i];

    return dst;
}


void Copy_Board_State_Reset(Board_State* a, Board_State* b){
    
    b->num_boxes = a->num_boxes;
    b->num_goals = a->num_goals;
    b->p1.x = a->p1.x;
    b->p1.y = a->p1.y;
    for (int i = 0; i < b->num_boxes; i++){
        b->goals[i] = a->goals[i];
        b->boxes[i] = a->boxes[i];
    }
}

bool Is_Game_Over_Handler(Board* b){
    if(Is_Game_Over(b->board_state)){
        Free_Board(b);
        return true;
    }
    return false;
}


/**
 *  blocage possible -> dans un coin
 *  contre un mur au long duquel
 *
 *
 *
 */ 
bool Is_Solvable(const Map m,const Board_State b){
    for(int i = 0; i < b->num_boxes; i++){
        
        int bi = b.boxes[i].x;
        int bj = b.boxes[j].y;
        
        // blocage contre le mur
        if(m[bi][bj+1] == WALL && m[bi+1][bj] == WALL ||   // coin inf droit
           m[bi][bj-1] == WALL && m[bi+1][bj] == WALL ||   
            
                ){

            return false;
        }


        // blocage con



    }
}


#include "board.h"

#define MAX_LINES 256


void Load_Level(Board* b, char* filename) {
    char* file_data = LoadFileText(filename);
    if (!file_data) {
        log_fatal("Level %s not found by %s.", filename, __func__);
    }

    char* lines[MAX_LINES];
    int line_count = 0;
    char* line = strtok(file_data, "\n");
    while (line != NULL && line_count < MAX_LINES) {
        lines[line_count++] = line;
        line = strtok(NULL, "\n");
    }

    int num_boxes = 0;
    int num_goals = 0;
    int section = 0;

    for (int i = 0; i < line_count; i++) {
        line = lines[i];
        if (strcmp(line, "[Board]") == 0) section = 1;
        else if (strcmp(line, "[Players]") == 0) section = 2;
        else if (strcmp(line, "[Boxes]") == 0) section = 3;
        else if (strcmp(line, "[Goals]") == 0) section = 4;
        else {
            if (section == 3 && strchr(line, '(')) num_boxes++;
            if (section == 4 && strchr(line, '(')) num_goals++;
        }
    }

    log_info("Found %d boxes and %d goals.", num_boxes, num_goals);

    b->board_state.boxes = malloc(num_boxes * sizeof(Box));
    b->board_state.goals = malloc(num_goals * sizeof(Goal));
    if (!b->board_state.boxes || !b->board_state.goals) {
        log_fatal("Memory allocation failed!");
    }

    b->board_state.num_boxes = 0;
    b->board_state.num_goals = 0;

    int row = 0;
    uint8_t file_x, file_y;
    section = 0;

    for (int i = 0; i < line_count; i++) {
        line = lines[i];

        if (strcmp(line, "[Board]") == 0) section = 1;
        else if (strcmp(line, "[Players]") == 0) section = 2;
        else if (strcmp(line, "[Boxes]") == 0) section = 3;
        else if (strcmp(line, "[Goals]") == 0) section = 4;
        else {
            switch (section) {
                case 1: // Board
                    if (row == 0) {
                        sscanf(line, "%hhu %hhu", &b->map.width, &b->map.height);
                        log_info("Grid -> Width: %d, Height: %d.", b->map.width, b->map.height);
                        b->map.tiles = (uint8_t**)malloc(b->map.height * sizeof(uint8_t*));
                        for (int i = 0; i < b->map.height; i++) {
                            b->map.tiles[i] = (uint8_t*)malloc(b->map.width * sizeof(uint8_t));
                        }
                        row++;
                    } else {
                        for (int col = 0; col < b->map.width; col++) {
                            b->map.tiles[row - 1][col] = (line[col] == '1') ? TILE_WALL : TILE_EMPTY;
                        }
                        row++;
                    }
                    break;

                case 2: // Player
                    if (sscanf(line, "(%hhu,%hhu)", &file_x, &file_y) == 2) {
                        b->board_state.p1.x = file_x;
                        b->board_state.p1.y = file_y;
                        log_info("Player position: %d,%d.", b->board_state.p1.x, b->board_state.p1.y);
                    }
                    break;

                case 3: // Boxes
                    if (sscanf(line, "(%hhu,%hhu)", &file_x, &file_y) == 2) {
                        int idx = b->board_state.num_boxes;
                        b->board_state.boxes[idx].x = file_x;
                        b->board_state.boxes[idx].y = file_y;
                        log_info("Box position: %d,%d.", file_x, file_y);
                        b->board_state.num_boxes++;
                    }
                    break;

                case 4: // Goals
                    if (sscanf(line, "(%hhu,%hhu)", &file_x, &file_y) == 2) {
                        int idx = b->board_state.num_goals;
                        b->board_state.goals[idx].x = file_x;
                        b->board_state.goals[idx].y = file_y;
                        log_info("Goal position: %d,%d.", file_x, file_y);
                        b->board_state.num_goals++;
                    }
                    break;

                default:
                    log_error("Unknown board specifier: %s.", line);
                    break;
            }
        }
    }

    log_info("Level parsing completed: %s.", filename);
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

void Free_Board_State(Board* b){
    free(b->board_state.boxes);
    free(b->board_state.goals);
    b->board_state.boxes = NULL;
    b->board_state.goals = NULL;
    b->board_state.num_boxes = 0;
    b->board_state.num_goals = 0;
}

void Free_Board(Board* b) {
    if (b->map.tiles) {
        for (int i = 0; i < b->map.height; i++) {
            free(b->map.tiles[i]);
        }
        free(b->map.tiles);
        b->map.tiles = NULL;
    }
    Free_Board_State(b->board_state);
}


void Copy_Board_State(Board_State* a, Board_State* b){
    
    b->num_boxes = a->num_boxes;
    b->num_goals = a->num_goals;
    b->p1.x = a->p1.x;
    b->p1.y = a->p1.y;
    b->goals = malloc(num_goals * sizeof(box));
    b->boxes = malloc(num_boxes * sizeof(goal));

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

void Play_Update(Board* b){
    if(IsKeyPressed(KEY_UP)){
        Make_Move(b->map, &b->board_state, MOVE_UP);
    } else if(IsKeyPressed(KEY_RIGHT)){
        Make_Move(b->map, &b->board_state, MOVE_RIGHT);
    } else if(IsKeyPressed(KEY_DOWN)){
        Make_Move(b->map, &b->board_state, MOVE_DOWN);
    } else if(IsKeyPressed(KEY_LEFT)){
        Make_Move(b->map, &b->board_state, MOVE_LEFT);
    }

}



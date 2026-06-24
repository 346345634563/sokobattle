#include "board.h"

void Load_Level(Board* b, char* filename){
    char* file_data = LoadFileText(filename);
    if(!file_data){
        log_fatal("Level %s not found by %s.", filename, __func__);
    }

    char* line = strtok(file_data, "\n");
    int section = 0;
    int row = 0;

    b->board_state.num_boxes = 0;
    b->board_state.num_goals = 0;

    while(line != NULL){

        if(strcmp(line, "[Board]") == 0){
            section = 1;
        } else if(strcmp(line, "[Players]") == 0){
            section = 2;
        } else if(strcmp(line, "[Boxes]") == 0){
            section = 3;
        } else if(strcmp(line, "[Goals]") == 0){
            section = 4;
        } else {


            switch(section){

                case 1: 

                    if(row == 0){
                        sscanf(line, "%hhu %hhu", &b->map.width, &b->map.height);

                        b->map.tiles = (uint8_t**)malloc(b->map.height * sizeof(uint8_t*));
                        for(int i = 0; i < b->map.height; i++){
                            b->map.tiles[i] = (uint8_t*)malloc(b->map.width * sizeof(uint8_t));
                        }
                        row++;

                    } else {
                        for(int col = 0; col < b->map.width; col++){
                            b->map.tiles[row-1][col] = (line[col] == '1') ? TILE_WALL : TILE_EMPTY;
                        }
                        row++;
                    }
                    break;

                case 2:                
                    uint8_t file_x, file_y;
                    if(sscanf(line, "(%hhu,%hhu)", &file_x, &file_y) == 2){
                        b->board_state.p1.x = file_x - 1;
                        b->board_state.p1.y = file_y - 1;
                    }
                    break;

                case 3:                
                    uint8_t file_x, file_y;
                    if(sscanf(line, "(%hhu,%hhu)", &file_x, &file_y) == 2){
                        int idx = b->board_state.num_boxes;
                        b->board_state.boxes[idx].x = file_x - 1;
                        b->board_state.boxes[idx].y = file_y - 1;
                        b->board_state.num_boxes++;
                    }
                    break;

                case 4: // Goals

                    uint8_t file_x, file_y;
                    if(sscanf(line, "(%hhu,%hhu)", &file_x, &file_y) == 2){
                        int idx = b->board_state.num_goals;
                        b->board_state.goals[idx].x = file_x - 1;
                        b->board_state.goals[idx].y = file_y - 1;
                        b->board_state.num_goals++;
                    }
                    break;

                default:
                    log_error("Unknown board specifier encountered in level file : %s.", line);
                    break;
            }
        }

        line = strtok(NULL, "\n");
    }

    UnloadFileText(file_data);
}




void Make_Move(Map map, Board_state* b, Move move){
    int dx = 0, dy = 0;
    switch(move){
        case MOVE_UP:    dx = -1; dy =  0; break;
        case MOVE_DOWN:  dx =  1; dy =  0; break;
        case MOVE_LEFT:  dx =  0; dy = -1; break;
        case MOVE_RIGHT: dx =  0; dy =  1; break;
        default: return;
    }

    uint8_t x = b->p1.x;
    uint8_t y = b->p1.y;

    int tx = x + dx;
    int ty = y + dy;


    // Wall check
    if(map.tiles[tx][ty] == TILE_WALL){
        return;
    }

    // Check for a box at target
    int box_idx = -1;
    for(int i = 0; i < b->num_boxes; i++){
        if(b->boxes[i].x == tx && b->boxes[i].y == ty){
            box_idx = i;
            break;
        }
    }

    if(box_idx >= 0){
        // There is a box, check the tile beyond
        int bx2 = tx + dx;
        int by2 = ty + dy;

        // Bounds check for box new position
        if(bx2 < 0 || bx2 >= map.height || by2 < 0 || by2 >= map.width){
            return;
        }

        // Check if that tile is a wall or another box
        if(map.tiles[bx2][by2] == TILE_WALL){
            return;
        }

        for(int i = 0; i < b->num_boxes; i++){
            if(i == box_idx){
                continue;
            }
            if(b->boxes[i].x == bx2 && b->boxes[i].y == by2){
                return; // another box blocks
            } 
        }

        // Move the box
        b->boxes[box_idx].x = bx2;
        b->boxes[box_idx].y = by2;
    }

    // Move the player
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
        if(!on_goal)
            return false;
    }
    return true;
}


void Free_Board(Board* b){
    free(b->tiles);
    free(b->board_state.boxes);
    free(b->board_state.goals);
}

void Is_Game_Over_Handler(Board* b){
    if(Is_Game_Over(b->board_state)){
        Free_Board(b);
        return true;
    }
    return false;
}

void Play_Update(Board* b){
    if(IsKeyPressed(KEY_UP)){
        Make_Move(b->map, b->board_state, MOVE_UP);
    } else if(IsKeyPressed(KEY_RIGHT)){
        Make_Move(b->map, b->board_state, MOVE_RIGHT);
    } else if(IsKeyPressed(KEY_DOWN)){
        Make_Move(b->map, b->board_state, MOVE_DOWN);
    } else if(IsKeyPressed(KEY_LEFT)){
        Make_Move(b->map, b->board_state, MOVE_LEFT);
    }

    if(Is_Game_Over(b->board_state)){
        log_info("Current level has ended");

    }
}


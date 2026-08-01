#include "playing.h"

Board current_board;
bool game_launched = false;

#define LEVEL_PATH "assets/levels/%02d.txt"
int current_level_number = 1;   

bool Playing_Load_Level(int level_number){
    char path[256];
    snprintf(path, sizeof(path), LEVEL_PATH, level_number);
    if (!FileExists(path)) {
        return false;
    }
    Load_Level(&current_board, path);
    current_level_number = level_number;
    return true;
}

void Playing_Init(){
    Playing_Load_Level(current_level_number);
}


void Playing_Board_Update(Board* b){
    if(IsKeyPressed(KEY_UP)){
        Make_Move(b->map, &b->board_state, MOVE_UP);
    } else if(IsKeyPressed(KEY_RIGHT)){
        Make_Move(b->map, &b->board_state, MOVE_RIGHT);
    } else if(IsKeyPressed(KEY_DOWN)){
        Make_Move(b->map, &b->board_state, MOVE_DOWN);
    } else if(IsKeyPressed(KEY_LEFT)){
        Make_Move(b->map, &b->board_state, MOVE_LEFT);
    // reset the board
    } else if(IsKeyPressed(KEY_R)){
        Copy_Board_State_Reset(&b->initial_board_state, &b->board_state);

    // solve
    } else if(IsKeyPressed(KEY_S)){
        Copy_Board_State_Reset(&b->initial_board_state, &b->board_state);
        Linked_List* solution = solve(b->map, b->board_state);
        
        if(solution->head == NULL){
            log_warn("Level is unsolvable %s",__func__);
        }

        while(solution->head != NULL){
            Move m = Linked_List_Pop(solution);
            Make_Move(b->map, &b->board_state, m);
            BeginDrawing();
            ClearBackground(RAYWHITE);

            Playing_Draw();
            EndDrawing();
            WaitTime(0.2);


        }
        free(solution);

    
    // return to menu
    } else if(IsKeyPressed(KEY_ESCAPE)){
        game_launched = false;

    }

}



void Playing_Update(){
    
    if(!game_launched){
        Playing_Init();
        game_launched = true;
    }
    Playing_Board_Update(&current_board);
    if(Is_Game_Over_Handler(&current_board)){
        log_info("Current level has ended, loading next level");
        current_level_number++;
        Playing_Init();
    }
}

void Playing_Draw(){
    Draw_Board(current_board.map,current_board.board_state);
    
}







 

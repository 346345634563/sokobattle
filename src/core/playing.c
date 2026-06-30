#include "playing.h"

Board current_board;
Playing_State ps; 
bool game_launched = false;

void Playing_Init(){
    ps = PLAYING_CURRENT;
    Load_Level(&current_board, "assets/levels/01.txt");
}


void Playing_Update(){
    
    if(!game_launched){
        Playing_Init();
        game_launched = true;
    }

    Play_Update(&current_board);
    if(Is_Game_Over_Handler(&current_board)){
        log_info("Current level has ended, loading next level");
    }
}

void Playing_Draw(){
    Draw_Board(current_board.map,current_board.board_state);

}







 

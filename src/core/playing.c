#include "playing.h"

static Board current_board;

void Playing_Init(void){
    Load_Level(&current_board, "assets/levels/01.txt");
}



void 

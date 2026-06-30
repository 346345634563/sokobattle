#ifndef PLAYING_H_
#define PLAYING_H_

#include "raylib.h"

#include "game_var.h"
#include "gameplay/board.h"

typedef enum{
    PLAYING_CURRENT,
    PLAYING_NEXT_LEVEL
}Playing_State;


void Playing_Init();
void Playing_Update();
void Playing_Draw();





#endif

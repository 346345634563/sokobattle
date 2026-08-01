#ifndef PLAYING_H_
#define PLAYING_H_

#include "raylib.h"

#include "game_var.h"
#include "gameplay/board.h"
#include "gameplay/solver.h"



void Playing_Init();
void Playing_Update();
void Playing_Draw();
void Playing_Board_Update(Board* b);




#endif

#ifndef GAME_H_
#define GAME_H_

#include "raylib.h"

#include "util/log.h"

#include "game_var.h"
#include "menu.h"
#include "playing.h"



typedef struct Game{
    int screenWidth;
    int screenHeight;
}Game;

void Game_Init(Game *g);
void Game_Run(Game *g);
void Game_Shutdown(Game *g);
void Game_Update(Game *g);
void Game_Render(Game *g);

#endif

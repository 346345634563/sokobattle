#ifndef MENU_H_
#define MENU_H_

#include "raylib.h"

#include "game_var.h"

enum{
    MENU_STATE_PLAY,
    MENU_STATE_MULTI,
    MENU_STATE_OPTION,
    MENU_STATE_QUIT,
    MENU_MAX
}; 

void Menu_Update();
void Menu_Draw();

#endif

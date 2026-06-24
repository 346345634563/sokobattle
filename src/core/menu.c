#include "menu.h"


void Menu_Update(){
    if(IsKeyPressed(KEY_DOWN)){
        if( GLOBAL_INTERFACE_INDEX < MENU_MAX){
            GLOBAL_INTERFACE_INDEX++;
            return; 
        }
    }

    if(IsKeyPressed(KEY_UP)){
        if(GLOBAL_INTERFACE_INDEX >= 0){
           GLOBAL_INTERFACE_INDEX--;
            return;
        }
    }

    if(IsKeyPressed(KEY_ENTER)){
        switch(GLOBAL_INTERFACE_INDEX){
            case MENU_STATE_PLAY:
                GLOBAL_GAME_STATE[0] = GAME_STATE_PLAYING;

                break;
            case MENU_STATE_OPTION:
                GLOBAL_GAME_STATE[0] = GAME_STATE_EXIT;
                break;
            case MENU_STATE_QUIT:
                GLOBAL_GAME_STATE[0] = GAME_STATE_EXIT;
                break;

        }

    }

}

void Menu_Draw(){
    DrawText("START GAME", 300, 150, 20, (GLOBAL_INTERFACE_INDEX == MENU_STATE_PLAY) ? YELLOW : RED);
    DrawText("OPTIONS", 300, 180, 20,(GLOBAL_INTERFACE_INDEX == MENU_STATE_OPTION) ? YELLOW : RED);
    DrawText("QUIT", 300, 210, 20,(GLOBAL_INTERFACE_INDEX == MENU_STATE_QUIT) ? YELLOW : RED); 

}

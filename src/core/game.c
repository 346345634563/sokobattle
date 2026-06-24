#include "game.h"

int GLOBAL_GAME_STATE[2] = {0, 0};
int GLOBAL_INTERFACE_INDEX = 0;


void Game_Init(Game *g){
    log_info("%s started", __func__);

    g->screenWidth = SCREEN_WIDTH;
    g->screenHeight = SCREEN_HEIGHT;
   
    // start at the first
    GLOBAL_GAME_STATE[0] = GAME_STATE_MENU;
    GLOBAL_INTERFACE_INDEX = 0;

    log_info("%s finished", __func__);
}

void Game_Run(Game *g){

    log_info("%s started", __func__);
    


    InitWindow(g->screenWidth, g->screenHeight, "Fripon");

    SetTargetFPS(30);

    while(!WindowShouldClose()){
        Game_Update(g);
        Game_Render(g);
    }    

    CloseWindow();
    
    log_info("%s finished", __func__);
}

void Game_Shutdown(Game *g){
    
}

void Game_Update(Game *g){
    switch(GLOBAL_GAME_STATE[0]){
        case GAME_STATE_MENU:
            Menu_Update();
            break;
        case GAME_STATE_PLAYING:
            Playing_Update();
            break;
        case GAME_STATE_PAUSE:
            break;
        case GAME_STATE_EXIT:
            break;
    }

}

void Game_Render(Game *g){

    BeginDrawing(); 
    ClearBackground(RAYWHITE);
    switch(GLOBAL_GAME_STATE[0]){
        case GAME_STATE_MENU:
            Menu_Draw();
            break;
        case GAME_STATE_PLAYING:

            
            break;
        case GAME_STATE_PAUSE:
            break;
        case GAME_STATE_EXIT:
            break;
    }
    EndDrawing();
}



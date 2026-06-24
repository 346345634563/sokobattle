#ifndef GAME_VAR_H_
#define GAME_VAR_H_

#define SCREEN_HEIGHT   400
#define SCREEN_WIDTH    750

extern int GLOBAL_GAME_STATE[2];
extern int GLOBAL_INTERFACE_INDEX;
extern int CURRENT_LEVEL;



typedef enum{
    GAME_STATE_MENU,
    GAME_STATE_PLAYING,
    GAME_STATE_PAUSE,
    GAME_STATE_EXIT
}GameState;


#endif

#include "util/log.h"
#include "core/game.h"
 

int main(){
   
   // Setting for RAYLIB logging
   SetTraceLogLevel(LOG_WARNING);

   log_info("Game started in main.c");

    Game g;

    Game_Init(&g);
    Game_Run(&g);
    Game_Shutdown(&g);

    log_info("Game exited from main.c");

    return 0;
}

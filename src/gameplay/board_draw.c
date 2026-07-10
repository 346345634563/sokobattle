#include "board.h"

  

void Draw_Board(const Map map, const Board_State state) {
   
    if(map.height == 0 || map.width == 0){
        log_error("map height or width are 0 in %s drawing can't be initialised",__func__);
        return;
    }
    const int TILE_SIZE = (int)( (SCREEN_HEIGHT > SCREEN_WIDTH) ? 
       SCREEN_HEIGHT / map.height : SCREEN_WIDTH / map.width) ;

    int rectangle_start_x =  SCREEN_WIDTH/2 - map.width*TILE_SIZE/2;
    int rectangle_start_y =  SCREEN_HEIGHT/2 - map.height*TILE_SIZE/2 ;
    for (int row = 0; row < map.height; row++) {
        for (int col = 0; col < map.width; col++) {
            // Calculate screen position (col = X, row = Y)
            Vector2 pos = { col * TILE_SIZE + rectangle_start_x, row * TILE_SIZE + rectangle_start_y};
            
            // Draw the floor (background tile) first
            DrawRectangle(pos.x, pos.y, TILE_SIZE, TILE_SIZE, LIGHTGRAY);
            
            if (map.tiles[row][col] == TILE_WALL) {
                DrawRectangle(pos.x, pos.y, TILE_SIZE, TILE_SIZE, DARKGRAY);
                DrawRectangleLines(pos.x, pos.y, TILE_SIZE, TILE_SIZE, GRAY);
            }
        }
    }

    for (int i = 0; i < state.num_goals; i++) {
        Vector2 pos = {
            state.goals[i].x * TILE_SIZE,
            state.goals[i].y * TILE_SIZE
        };
        DrawCircle(pos.x + TILE_SIZE/2 + rectangle_start_x, pos.y + TILE_SIZE/2 + rectangle_start_y, 10, GOLD);
       
    }

    for (int i = 0; i < state.num_boxes; i++) {
        Vector2 pos = {
            state.boxes[i].x * TILE_SIZE,
            state.boxes[i].y * TILE_SIZE
        };
        
        DrawRectangle(pos.x + 4 + rectangle_start_x, pos.y + 4 + rectangle_start_y, TILE_SIZE - 8, TILE_SIZE - 8, BROWN);
        
    }

    Vector2 player_pos = {
        state.p1.x * TILE_SIZE,
        state.p1.y * TILE_SIZE
    };

    DrawCircle(player_pos.x + TILE_SIZE/2 + rectangle_start_x, player_pos.y + TILE_SIZE/2 + rectangle_start_y, TILE_SIZE/2 - 6, BLUE);
    
}

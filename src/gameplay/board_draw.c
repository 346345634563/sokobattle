#include "board.h"

#define TILE_SIZE 60 

void Draw_Board(const Map map, const Board_State state) {

    for (int row = 0; row < map.height; row++) {
        for (int col = 0; col < map.width; col++) {
            // Calculate screen position (col = X, row = Y)
            Vector2 pos = { col * TILE_SIZE, row * TILE_SIZE };
            
            // Draw the floor (background tile) first
            DrawRectangle(pos.x, pos.y, TILE_SIZE, TILE_SIZE, LIGHTGRAY);
            
            // Draw walls on top of the floor
            if (map.tiles[row][col] == TILE_WALL) {
                DrawRectangle(pos.x, pos.y, TILE_SIZE, TILE_SIZE, DARKGRAY);
                // Optional: add a border to walls for visual clarity
                DrawRectangleLines(pos.x, pos.y, TILE_SIZE, TILE_SIZE, GRAY);
            }
        }
    }

    for (int i = 0; i < state.num_goals; i++) {
        Vector2 pos = {
            state.goals[i].x * TILE_SIZE,
            state.goals[i].y * TILE_SIZE
        };
        DrawCircle(pos.x + TILE_SIZE/2, pos.y + TILE_SIZE/2, 10, GOLD);
        DrawCircleLines(pos.x + TILE_SIZE/2, pos.y + TILE_SIZE/2, 14, ORANGE);
    }

    for (int i = 0; i < state.num_boxes; i++) {
        Vector2 pos = {
            state.boxes[i].x * TILE_SIZE,
            state.boxes[i].y * TILE_SIZE
        };
        
        DrawRectangle(pos.x + 4, pos.y + 4, TILE_SIZE - 8, TILE_SIZE - 8, BROWN);
        DrawRectangleLines(pos.x + 2, pos.y + 2, TILE_SIZE - 4, TILE_SIZE - 4, DARKBROWN);
        
        DrawLine(pos.x + 10, pos.y + 10, pos.x + TILE_SIZE - 10, pos.y + TILE_SIZE - 10, DARKBROWN);
        DrawLine(pos.x + TILE_SIZE - 10, pos.y + 10, pos.x + 10, pos.y + TILE_SIZE - 10, DARKBROWN);
    }

    Vector2 player_pos = {
        state.p1.x * TILE_SIZE,
        state.p1.y * TILE_SIZE
    };

    DrawCircle(player_pos.x + TILE_SIZE/2, player_pos.y + TILE_SIZE/2, TILE_SIZE/2 - 6, BLUE);
    DrawCircleLines(player_pos.x + TILE_SIZE/2, player_pos.y + TILE_SIZE/2, TILE_SIZE/2 - 6, DARKBLUE);
    
}

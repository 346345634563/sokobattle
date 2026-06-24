#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"
#include "../util/log.h"

typedef enum {
    TILE_EMPTY = 0,
    TILE_WALL = 1,
    TILE_VOID = 2
} Tile;

typedef enum {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT
} Move;

// 0,0 -> the uppermost left tile
typedef struct player {
    uint8_t x;
    uint8_t y;
} Player;

typedef struct box {
    uint8_t x;
    uint8_t y;
} Box;

typedef struct goal {
    uint8_t x;
    uint8_t y;
} Goal;

typedef struct board_state {
    Player p1;
    uint8_t num_boxes;
    uint8_t num_goals;
    Box* boxes;      // Dynamically allocated array
    Goal* goals;     // Dynamically allocated array
} Board_State;

typedef struct map {
    uint8_t height;
    uint8_t width;
    uint8_t** tiles; // 2D array of Tile enum values
} Map;

typedef struct board {
    Map map;
    Board_State board_state;
} Board;


void Load_Level(Board* b, char* filename);
void Make_Move(Map map, Board_State* b, Move move);
bool Is_Game_Over(Board_State b);
bool Is_Game_Over_Handler(Board* b);
void Play_Update(Board* b);
void Free_Board(Board* b);


#endif // BOARD_H

#include "board.h"

#define MAX_LINES 256

void Compute_Dead_Tiles(Map* map, const Board_State* initial) {
    int w = map->width, h = map->height;
    int size = w * h;

    bool* visited = calloc(size, sizeof(bool));
    int* queue = malloc(size * sizeof(int));
    int head = 0, tail = 0;

    for (int i = 0; i < initial->num_goals; i++) {
        int x = initial->goals[i].x;
        int y = initial->goals[i].y;
        int pos = y * w + x;
        if (!visited[pos]) {
            visited[pos] = true;
            queue[tail++] = pos;
        }
    }

    if (tail == 0) {
        map->dead = malloc(h * sizeof(bool*));
        for (int y = 0; y < h; y++) {
            map->dead[y] = malloc(w * sizeof(bool));
            for (int x = 0; x < w; x++) map->dead[y][x] = true;
        }
        free(visited); free(queue);
        return;
    }

    int dx[4] = {0, 0, -1, 1};
    int dy[4] = {-1, 1, 0, 0};

    while (head < tail) {
        int pos = queue[head++];
        int bx = pos % w;
        int by = pos / w;

        for (int d = 0; d < 4; d++) {
            int nx = bx + dx[d];
            int ny = by + dy[d];
            int px = bx - dx[d];
            int py = by - dy[d];

            if (nx >= 0 && nx < w && ny >= 0 && ny < h && map->tiles[ny][nx] != TILE_WALL &&
                px >= 0 && px < w && py >= 0 && py < h && map->tiles[py][px] != TILE_WALL) {
                int npos = ny * w + nx;
                if (!visited[npos]) {
                    visited[npos] = true;
                    queue[tail++] = npos;
                }
            }
        }
    }

    map->dead = malloc(h * sizeof(bool*));
    for (int y = 0; y < h; y++) {
        map->dead[y] = malloc(w * sizeof(bool));
        for (int x = 0; x < w; x++) {
            map->dead[y][x] = !visited[y * w + x];
        }
    }

    printf("Dead tiles visualization (X = dead, . = live, # = wall):\n");
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            if (map->tiles[y][x] == TILE_WALL) {
                printf("#");
            } else if (map->dead[y][x]) {
                printf("X");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }


    free(visited);
    free(queue);
}


void Load_Level(Board* b, const char* filename) {
    char* file_data = LoadFileText(filename);
    if (!file_data) {
        log_fatal("Cannot load level file: %s", filename);
    }

    char* lines[256]; 
    int line_count = 0;
    char* line = strtok(file_data, "\n");
    while (line != NULL && line_count < 256) {
        if (strlen(line) > 0 && line[0] != ';') {   
            lines[line_count++] = line;
        }
        line = strtok(NULL, "\n");
    }

    if (line_count == 0) {
        log_fatal("No valid lines in level file.");
    }

    int width = 0;
    for (int i = 0; i < line_count; i++) {
        int len = strlen(lines[i]);
        if (len > width) width = len;
    }
    int height = line_count;

    b->map.width = width;
    b->map.height = height;
    b->map.tiles = (uint8_t**)malloc(height * sizeof(uint8_t*));
    for (int y = 0; y < height; y++) {
        b->map.tiles[y] = (uint8_t*)malloc(width * sizeof(uint8_t));
        // Par défaut : vide
        for (int x = 0; x < width; x++) {
            b->map.tiles[y][x] = TILE_EMPTY;
        }
    }

    int num_boxes = 0, num_goals = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char c = (x < strlen(lines[y])) ? lines[y][x] : ' ';
            if (c == '$' || c == '*') num_boxes++;
            if (c == '.' || c == '*' || c == '+') num_goals++;
        }
    }

    b->board_state.num_boxes = 0;
    b->board_state.num_goals = 0;
    b->board_state.boxes = malloc(num_boxes * sizeof(Box));
    b->board_state.goals = malloc(num_goals * sizeof(Goal));
    if (!b->board_state.boxes || !b->board_state.goals) {
        log_fatal("Memory allocation failed for boxes/goals.");
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            char c = (x < strlen(lines[y])) ? lines[y][x] : ' ';
            // Mur
            if (c == '#') {
                b->map.tiles[y][x] = TILE_WALL;
            }
            // Joueur
            else if (c == '@' || c == '+') {
                b->board_state.p1.x = x;
                b->board_state.p1.y = y;
                if (c == '+') {
                    b->map.tiles[y][x] = TILE_EMPTY;  
                }
            }
            else if (c == '$' || c == '*') {
                int idx = b->board_state.num_boxes++;
                b->board_state.boxes[idx].x = x;
                b->board_state.boxes[idx].y = y;
            }
            if (c == '.' || c == '*' || c == '+') {
                int idx = b->board_state.num_goals++;
                b->board_state.goals[idx].x = x;
                b->board_state.goals[idx].y = y;
            }
        }
    }
    
    b->initial_board_state.boxes = malloc(b->board_state.num_boxes * sizeof(Box));
    b->initial_board_state.goals = malloc(b->board_state.num_goals * sizeof(Goal));
    if (!b->initial_board_state.boxes || !b->initial_board_state.goals) {
        log_fatal("Memory allocation failed for initial state");
    }
    Copy_Board_State_Reset(&b->board_state, &b->initial_board_state);

    Compute_Dead_Tiles(&b->map, &b->initial_board_state);


    log_info("Level loaded: width=%d, height=%d, boxes=%d, goals=%d",
             width, height, b->board_state.num_boxes, b->board_state.num_goals);

    UnloadFileText(file_data);
}



void Make_Move(Map map, Board_State* b, Move move){
    int dx = 0, dy = 0;
    switch(move){
        case MOVE_UP:    dx =  0; dy = -1; break;
        case MOVE_DOWN:  dx =  0; dy =  1; break;
        case MOVE_LEFT:  dx = -1; dy =  0; break;
        case MOVE_RIGHT: dx =  1; dy =  0; break;
        default: return;
    }

    uint8_t x = b->p1.x;
    uint8_t y = b->p1.y;

    int tx = x + dx; 
    int ty = y + dy; 

    if (tx < 0 || tx >= map.width || ty < 0 || ty >= map.height)
        return;

    if(map.tiles[ty][tx] == TILE_WALL){
        return;
    }

    int box_idx = -1;
    for(int i = 0; i < b->num_boxes; i++){
        if(b->boxes[i].x == tx && b->boxes[i].y == ty){
            box_idx = i;
            break;
        }
    }

    if(box_idx >= 0){
        int bx2 = tx + dx; 
        int by2 = ty + dy; 

        if(bx2 < 0 || bx2 >= map.width || by2 < 0 || by2 >= map.height)
            return;

        if(map.tiles[by2][bx2] == TILE_WALL)
            return;

        for(int i = 0; i < b->num_boxes; i++){
            if(i == box_idx) continue;
            if(b->boxes[i].x == bx2 && b->boxes[i].y == by2)
                return;
        }

        b->boxes[box_idx].x = bx2;
        b->boxes[box_idx].y = by2;
    }

    b->p1.x = tx;
    b->p1.y = ty;
}



bool Is_Game_Over(Board_State b){

    for(int i = 0; i < b.num_boxes; i++){
        bool on_goal = false;
        for(int j = 0; j < b.num_goals; j++){
            if(b.boxes[i].x == b.goals[j].x && b.boxes[i].y == b.goals[j].y){
                on_goal = true;
                break;
            }
        }
        if(!on_goal){
            return false;
        }
    }
    return true;
}

void Free_Board_State(Board_State* b){
    free(b->boxes);
    free(b->goals);
    b->boxes = NULL;
    b->goals = NULL;
    b->num_boxes = 0;
    b->num_goals = 0;
}

void Free_Board(Board* b) {
    if (b->map.tiles) {
        for (int i = 0; i < b->map.height; i++) {
            free(b->map.tiles[i]);
        }
        free(b->map.tiles);
        b->map.tiles = NULL;
    }
   if (b->map.dead) {
        for (int y = 0; y < b->map.height; y++) {
            free(b->map.dead[y]);
        }
        free(b->map.dead);
        b->map.dead = NULL;
    }

    Free_Board_State(&b->board_state);
}


Board_State* Copy_Board_State(const Board_State* src) {
    Board_State* dst = malloc(sizeof(Board_State));
    if (!dst) return NULL;

    dst->num_boxes = src->num_boxes;
    dst->num_goals = src->num_goals;
    dst->p1 = src->p1;

    dst->boxes = malloc(src->num_boxes * sizeof(Box));
    dst->goals = malloc(src->num_goals * sizeof(Goal));

    if (!dst->boxes || !dst->goals) {
        free(dst->boxes);
        free(dst->goals);
        free(dst);
        return NULL;
    }

    for (int i = 0; i < src->num_boxes; i++)
        dst->boxes[i] = src->boxes[i];
    for (int i = 0; i < src->num_goals; i++)
        dst->goals[i] = src->goals[i];

    return dst;
}


void Copy_Board_State_Reset(Board_State* a, Board_State* b){
    
    b->num_boxes = a->num_boxes;
    b->num_goals = a->num_goals;
    b->p1.x = a->p1.x;
    b->p1.y = a->p1.y;
    for (int i = 0; i < b->num_boxes; i++){
        b->goals[i] = a->goals[i];
        b->boxes[i] = a->boxes[i];
    }
}

bool Is_Game_Over_Handler(Board* b){
    if(Is_Game_Over(b->board_state)){
        Free_Board(b);
        return true;
    }
    return false;
}

/*
// Check if a box is blocked along the horizontal axis
bool Is_Box_Blocked_Horizontal(const Map* map, const Board_State* state, 
                                int box_x, int box_y, bool* frozen_boxes) {
    // Check left and right sides
    bool left_blocked = false;
    bool right_blocked = false;
    
    // Left side
    if (box_x - 1 < 0 || map->tiles[box_y][box_x - 1] == TILE_WALL) {
        left_blocked = true;
    } else {
        // Check if there's a box on the left that's frozen
        for (int i = 0; i < state->num_boxes; i++) {
            if (state->boxes[i].x == box_x - 1 && state->boxes[i].y == box_y) {
                if (frozen_boxes[i]) left_blocked = true;
                break;
            }
        }
        // Or if it's a dead square
        if (!left_blocked && map->dead[box_y][box_x - 1]) left_blocked = true;
    }
    
    // Right side
    if (box_x + 1 >= map->width || map->tiles[box_y][box_x + 1] == TILE_WALL) {
        right_blocked = true;
    } else {
        for (int i = 0; i < state->num_boxes; i++) {
            if (state->boxes[i].x == box_x + 1 && state->boxes[i].y == box_y) {
                if (frozen_boxes[i]) right_blocked = true;
                break;
            }
        }
        if (!right_blocked && map->dead[box_y][box_x + 1]) right_blocked = true;
    }
    
    return left_blocked && right_blocked;
}

// Check if a box is blocked along the vertical axis
bool Is_Box_Blocked_Vertical(const Map* map, const Board_State* state,
                              int box_x, int box_y, bool* frozen_boxes) {
    bool up_blocked = false;
    bool down_blocked = false;
    
    // Up side
    if (box_y - 1 < 0 || map->tiles[box_y - 1][box_x] == TILE_WALL) {
        up_blocked = true;
    } else {
        for (int i = 0; i < state->num_boxes; i++) {
            if (state->boxes[i].x == box_x && state->boxes[i].y == box_y - 1) {
                if (frozen_boxes[i]) up_blocked = true;
                break;
            }
        }
        if (!up_blocked && map->dead[box_y - 1][box_x]) up_blocked = true;
    }
    
    // Down side
    if (box_y + 1 >= map->height || map->tiles[box_y + 1][box_x] == TILE_WALL) {
        down_blocked = true;
    } else {
        for (int i = 0; i < state->num_boxes; i++) {
            if (state->boxes[i].x == box_x && state->boxes[i].y == box_y + 1) {
                if (frozen_boxes[i]) down_blocked = true;
                break;
            }
        }
        if (!down_blocked && map->dead[box_y + 1][box_x]) down_blocked = true;
    }
    
    return up_blocked && down_blocked;
}

// Main freeze deadlock detection - recursive because boxes can block each other[reference:5]
bool Is_Freeze_Deadlock(const Map* map, const Board_State* state) {
    int num_boxes = state->num_boxes;
    bool* frozen = calloc(num_boxes, sizeof(bool));
    bool changed = true;
    
    // Iteratively mark frozen boxes (boxes can freeze other boxes)
    while (changed) {
        changed = false;
        for (int i = 0; i < num_boxes; i++) {
            if (frozen[i]) continue;
            
            int bx = state->boxes[i].x;
            int by = state->boxes[i].y;
            
            bool horiz_blocked = Is_Box_Blocked_Horizontal(map, state, bx, by, frozen);
            bool vert_blocked = Is_Box_Blocked_Vertical(map, state, bx, by, frozen);
            
            if (horiz_blocked && vert_blocked) {
                frozen[i] = true;
                changed = true;
            }
        }
    }
    
    // If any frozen box is NOT on a goal → deadlock[reference:6]
    for (int i = 0; i < num_boxes; i++) {
        if (!frozen[i]) continue;
        
        bool on_goal = false;
        for (int j = 0; j < state->num_goals; j++) {
            if (state->boxes[i].x == state->goals[j].x && 
                state->boxes[i].y == state->goals[j].y) {
                on_goal = true;
                break;
            }
        }
        if (!on_goal) {
            log_info("Freeze deadlock detected: box %d at (%d,%d)", i, state->boxes[i].x, state->boxes[i].y);

            free(frozen);
            return true;  // Deadlock!
        }
    }
    
    free(frozen);
    return false;
}
*/
bool Is_Wall_Blocked(const Map m, const Board_State b) {
    for (int i = 0; i < b.num_boxes; i++) {
        int x = b.boxes[i].x;
        int y = b.boxes[i].y;
        if (m.dead[y][x]) {  
            return true;
        }
    }
    return false;
}




bool Is_Solvable(const Map m, const Board_State b) {
    return !Is_Wall_Blocked(m, b);   
}


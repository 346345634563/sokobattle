#include "solver.h"







Board_State* Make_Move_Solver(Map map, Board_State* a, Move move){
    
    Board_State* b = Copy_Board_State(a);

    int dx = 0, dy = 0;
    switch(move){
        case MOVE_UP:    dx =  0; dy = -1; break;
        case MOVE_DOWN:  dx =  0; dy =  1; break;
        case MOVE_LEFT:  dx = -1; dy =  0; break;
        case MOVE_RIGHT: dx =  1; dy =  0; break;
        default: return NULL;
    }

    uint8_t x = b->p1.x;
    uint8_t y = b->p1.y;

    int tx = x + dx; 
    int ty = y + dy; 

    if (tx < 0 || tx >= map.width || ty < 0 || ty >= map.height){
        return NULL;
    }

    if(map.tiles[ty][tx] == TILE_WALL){
        return NULL;
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

        if(bx2 < 0 || bx2 >= map.width || by2 < 0 || by2 >= map.height){
            return NULL;
        }

        if(map.tiles[by2][bx2] == TILE_WALL){
            return NULL;
        }

        for(int i = 0; i < b->num_boxes; i++){
            if(i == box_idx){
                continue;
            }

            if(b->boxes[i].x == bx2 && b->boxes[i].y == by2){
                return NULL;
            }

        }

        b->boxes[box_idx].x = bx2;
        b->boxes[box_idx].y = by2;
    }

    b->p1.x = tx;
    b->p1.y = ty;

    return b;
}


Linked_List* solve(const Map m, Board_State b) {
    log_info("Attempting to solve");

    int states_created = 0;
    int states_explored = 0;

    Board_State* init_copy = Copy_Board_State(&b);
    if (!init_copy) return NULL;

    Prio_Queue pq;
    Prio_Queue_Init(&pq);
    Prio_Queue_Add(&pq, NULL, init_copy, 0, heuristic(init_copy), MOVE_COUNT);

    Hashset visited;
    Hashset_Init(&visited);
    Hashset_Add(&visited, init_copy); 

    Node_Prio* current;
    Node_Prio* goal_node = NULL;

    while (pq.size > 0) {
        current = Prio_Queue_Pop(&pq);
        states_explored++;
        if (Is_Game_Over(*current->board_state)) {
            goal_node = current;
            break;
        }

        for (int i = 0; i < MOVE_COUNT; i++) {
            Move move = (Move)i;
            Board_State* new_state = Make_Move_Solver(m, current->board_state, move);
           
            if (!new_state) {
                continue;
            }

            /*
            if (Is_Freeze_Deadlock(&m, new_state)) {
                Free_Board_State(new_state);
                free(new_state);
                log_info("deadlock skipped");
                continue;
            }
            */

            if (Hashset_Contains(&visited, new_state)) {
                Free_Board_State(new_state);
                free(new_state);

                continue;
            }

            int new_g = current->g + 1;
            int new_h = heuristic(new_state);
            
            states_created++;
            Prio_Queue_Add(&pq, current, new_state, new_g, new_h, move);
            Hashset_Add(&visited, new_state);
        }
    }

    Linked_List* solution = malloc(sizeof(Linked_List));
    if (!solution) {
        Prio_Queue_Clean(&pq);
        Hashset_Clean(&visited);
        return NULL;
    }
    Linked_List_Init(solution);

    if (goal_node) {
        Node_Prio* cur = goal_node;
        while (cur->parent != NULL) {
            Linked_List_Append(solution, cur->m);
            cur = cur->parent;
        }
    }

    Prio_Queue_Clean(&pq);
    Hashset_Clean(&visited);

    log_info("created : %d, explored: %d", states_created, states_explored);

    return solution;
}


int manhattan(int x1, int y1, int x2, int y2){
    int x = (x1 > x2) ? x1 - x2 : x2 - x1;
    int y = (y1 > y2) ? y1 - y2 : y2 - y1;
    return x + y;
}


static int hungarian(int** cost, int n, int* assignment) {
    int* u = (int*)calloc(n + 1, sizeof(int));
    int* v = (int*)calloc(n + 1, sizeof(int));
    int* p = (int*)calloc(n + 1, sizeof(int));
    int* way = (int*)calloc(n + 1, sizeof(int));

    for (int i = 1; i <= n; i++) {
        p[0] = i;
        int j0 = 0;
        int* minv = (int*)malloc((n + 1) * sizeof(int));
        char* used = (char*)calloc(n + 1, sizeof(char));
        for (int j = 0; j <= n; j++) minv[j] = INT_MAX;
        do {
            used[j0] = 1;
            int i0 = p[j0];
            int delta = INT_MAX;
            int j1 = 0;
            for (int j = 1; j <= n; j++) {
                if (!used[j]) {
                    int cur = cost[i0 - 1][j - 1] - u[i0] - v[j];
                    if (cur < minv[j]) { minv[j] = cur; way[j] = j0; }
                    if (minv[j] < delta) { delta = minv[j]; j1 = j; }
                }
            }
            for (int j = 0; j <= n; j++) {
                if (used[j]) { u[p[j]] += delta; v[j] -= delta; }
                else { minv[j] -= delta; }
            }
            j0 = j1;
        } while (p[j0] != 0);
        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
        free(minv);
        free(used);
    }
    if (assignment) {
        for (int j = 1; j <= n; j++) {
            if (p[j] > 0) assignment[p[j] - 1] = j - 1; // box i -> goal j
        }
    }
    int result = -v[0];
    free(u); free(v); free(p); free(way);
    return result;
}


int heuristic(const Board_State* b) {
    int n = b->num_boxes;
    int m = b->num_goals;
    if (n != m) {
        int h = 0;
        for (int i = 0; i < n; i++) {
            int min_d = INT_MAX;
            for (int j = 0; j < m; j++) {
                int d = manhattan(b->boxes[i].x, b->boxes[i].y,
                                  b->goals[j].x, b->goals[j].y);
                if (d < min_d) min_d = d;
            }
            h += min_d;
        }
        return h;
    }
    if (n == 0) return 0;

    int** cost = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; i++) {
        cost[i] = (int*)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) {
            cost[i][j] = manhattan(b->boxes[i].x, b->boxes[i].y,
                                   b->goals[j].x, b->goals[j].y);
        }
    }

    // 2. Affectation optimale
    int* assignment = (int*)malloc(n * sizeof(int));
    int base_cost = hungarian(cost, n, assignment); // coût total caisse->objectif

    // 3. Ajouter le coût du joueur pour chaque caisse affectée
    int player_cost = 0;
    for (int i = 0; i < n; i++) {
        int goal_idx = assignment[i];
        int bx = b->boxes[i].x;
        int by = b->boxes[i].y;
        int gx = b->goals[goal_idx].x;
        int gy = b->goals[goal_idx].y;

        // Direction de poussée idéale (de la caisse vers l'objectif)
        int dx = (gx > bx) ? 1 : (gx < bx) ? -1 : 0;
        int dy = (gy > by) ? 1 : (gy < by) ? -1 : 0;
        // Si la caisse est déjà sur l'objectif, pas de direction (dx=dy=0)
        if (dx == 0 && dy == 0) {
            // La caisse est déjà sur un objectif, le joueur n'a pas à la pousser.
            // On pourrait ajouter 0, ou alors le joueur doit peut-être se déplacer pour d'autres.
            continue;
        }
        // Position de poussée : derrière la caisse (côté opposé à l'objectif)
        int px = bx - dx;
        int py = by - dy;
        // Distance du joueur à cette position de poussée
        int dist_player = manhattan(b->p1.x, b->p1.y, px, py);
        player_cost += dist_player;
    }

    // Libération
    for (int i = 0; i < n; i++) free(cost[i]);
    free(cost);
    free(assignment);

    // L'heuristique totale est la somme du coût de déplacement des caisses
    // et du coût pour le joueur d'atteindre les positions de poussée.
    return base_cost + player_cost;
}

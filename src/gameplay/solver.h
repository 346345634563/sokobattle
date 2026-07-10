#ifndef SOLVER_H_
#define SOLVER_H_

#include "util/linked_list.h"
#include "util/prio_queue.h"
#include "util/hashset.h"
#include "util/log.h"

#include "limits.h"
#include "board.h"


Board_State* Make_Move_Solver(Map map, Board_State* a, Move move);

Linked_List* solve(const Map m, Board_State b);
int heuristic(const Board_State b);


#endif

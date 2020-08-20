//
// Created by Kliment Serafimov on 8/16/20.
//

#ifndef COMPACTPOSET_PRIMITIVERULE_H
#define COMPACTPOSET_PRIMITIVERULE_H

#include <string>
#include <vector>
#include "BittreeNode.h"
#include "AutomatonRuleCost.h"

enum PrimitiveRule {inherit_from_parent, stay, move_right,   move_left, move_up, move_down,   move_front, move_back, plus_one,   minus_one,  youngest_second_nephew};
static const string rule_names[25] =
        {"inherit_from_parent", "stay", "move_right",   "move_left", "move_up", "move_down",   "move_front", "move_back", "copy",   "erase", "youngest_second_nephew"};
static const int rule_cost[20] = {0, 0, 1,  2, 1, 2,  1, 2, 3, 1};


#endif //COMPACTPOSET_PRIMITIVERULE_H

//
// Created by Kliment Serafimov on 8/16/20.
//

#ifndef COMPACTPOSET_PRIMITIVERULE_H
#define COMPACTPOSET_PRIMITIVERULE_H

#include <string>
#include <vector>
#include "BittreeNode.h"
#include "AutomatonRuleCost.h"

enum PrimitiveRule {inherit_from_parent, stay, move_right, move_left, move_to_last_copy, copy_right, copy_left, move_up, move_down, move_front, move_back};
static const string rule_names[25] =
        {"inherit_from_parent", "stay", "move_right", "move_left", "move_to_last_copy", "copy_right", "copy_left", "move_up", "move_down", "move_front", "move_back"};
static const int rule_cost[20] = {0, 1, 2, 2, 100, 100, 100, 2, 2, 2, 2};


#endif //COMPACTPOSET_PRIMITIVERULE_H

//
// Created by Kliment Serafimov on 8/13/20.
//

#ifndef COMPACTPOSET_AUTOMATONRULECOST_H
#define COMPACTPOSET_AUTOMATONRULECOST_H

#include "string"

using namespace std;

class AutomatonRuleCost
{
    bool defined = false;
    int cost = 0;
public:
    AutomatonRuleCost() = default;

    AutomatonRuleCost(int _cost)
    {
        defined = true;
        cost = _cost;
    }

    int get_cost()
    {
        return cost;
    }

    void add_cost(AutomatonRuleCost _cost) {
        defined = true;
        cost += _cost.get_cost();
    }

    bool operator != (const AutomatonRuleCost& other) const
    {
        return cost != other.cost;
    }

    bool operator < (const AutomatonRuleCost& other) const
    {
        return cost < other.cost;
    }

    bool operator >= (const int& other_int) const
    {
        return cost >= other_int;
    }

    bool get_defined() {
        return defined;
    }

    string to_string()
    {
        return std::to_string(cost);
    }
};

#include <string>
#include <vector>
#include "BittreeNode.h"

#endif //COMPACTPOSET_AUTOMATONRULECOST_H

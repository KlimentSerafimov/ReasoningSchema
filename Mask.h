//
// Created by Kliment Serafimov on 8/10/20.
//

#ifndef COMPACTPOSET_MASK_H
#define COMPACTPOSET_MASK_H


#include "Bitvector.h"
#include <utility>
#include <vector>

class Mask: public Bitvector
{
public:
    Mask() = default;
    Mask(Bitvector _bitvector) : Bitvector(_bitvector) {}
};

class MaskAndCost: public Mask
{
    vector<MaskAndCost> local_variety;
public:
    int cost;
    MaskAndCost() = default;
    MaskAndCost(int _cost, Bitvector _bitvector) : Mask(_bitvector)
    {
        cost = _cost;
    }
    bool operator < (const MaskAndCost & other) const
    {
        if(cost != other.cost)
        {
            return cost < other.cost;
        }
        else
        {
            return Mask::operator<(other);
        }
    }
    void set_local_variety(vector<MaskAndCost> _local_variety)
    {
        local_variety = std::move(_local_variety);
    }
};

#endif //COMPACTPOSET_MASK_H

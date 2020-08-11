//
// Created by Kliment Serafimov on 8/10/20.
//

#ifndef COMPACTPOSET_MASK_H
#define COMPACTPOSET_MASK_H


#include "Bitvector.h"
#include <utility>
#include <vector>
#include "CanvasAndBittreeProgram.h"

class Mask: public Bitvector
{
public:
    Mask() = default;
    Mask(Bitvector _bitvector) : Bitvector(_bitvector) {}
    Mask(Mask * _bitvector) : Bitvector(*_bitvector) {}
};

class MaskAndCost: public Mask
{
public:
    CanvasAndBittreeProgram* program = nullptr;
    vector<MaskAndCost> local_variety;
    int cost;
    MaskAndCost() = default;
    MaskAndCost(MaskAndCost * to_copy) : Mask(to_copy)
    {
        cost = to_copy->cost;
        local_variety = to_copy->local_variety;
    }
    MaskAndCost(int _cost, Bitvector _bitvector) : Mask(_bitvector)
    {
        cost = _cost;
    }
    MaskAndCost(CanvasAndBittreeProgram* _program);

    virtual bool operator < (const MaskAndCost & other) const
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

class MaskAndCostSortByMask: public MaskAndCost
{
public:
    MaskAndCostSortByMask(MaskAndCost* to_copy) : MaskAndCost(to_copy)
    {

    }

    bool operator < (const MaskAndCost & other) const override
    {
        return Mask::operator<(other);
    }
};

#endif //COMPACTPOSET_MASK_H

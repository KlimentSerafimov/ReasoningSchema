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
    BittreeTaskTypeAsPartialFunction* now_canvas = nullptr;
    BittreeTaskTypeAsPartialFunction* next_canvas = nullptr;
    CanvasAndBittreeProgram* program = nullptr;
    vector<MaskAndCost> local_variety;
    int cost;
    pair<MaskAndCost*, int> best_edge;
    MaskAndCost() = default;
    MaskAndCost(MaskAndCost * to_copy) : Mask(to_copy)
    {
        cost = to_copy->cost;
        local_variety = to_copy->local_variety;
        program = to_copy->program;
        now_canvas = to_copy->now_canvas;
        next_canvas = to_copy->next_canvas;
        best_edge = make_pair(nullptr, -1);
    }
    MaskAndCost(int _cost, Bitvector _bitvector) : Mask(_bitvector)
    {
        cost = _cost;
        best_edge = make_pair(nullptr, -1);
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
    string to_string()
    {
        return Mask::to_string();
    }
    void set_next_bittree_as_partial(BittreeTaskTypeAsPartialFunction* bittree_as_partial)
    {
        next_canvas = bittree_as_partial;
    }

    void set_now_bittree_as_partial(BittreeTaskTypeAsPartialFunction *pFunction);

    void set_best_edge(MaskAndCost *pCost, int i);
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

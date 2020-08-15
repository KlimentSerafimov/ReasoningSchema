//
// Created by Kliment Serafimov on 8/10/20.
//

#ifndef COMPACTPOSET_MASK_H
#define COMPACTPOSET_MASK_H


#include "Bitvector.h"
#include <utility>
#include <vector>
#include "CanvasAndBittreeProgram.h"
#include "AutomatonRuleCost.h"

class Mask: public Bitvector
{
public:
    Mask() = default;
    Mask(Bitvector _bitvector) : Bitvector(_bitvector) {}
    Mask(Mask * _bitvector) : Bitvector(*_bitvector) {}
};

class MaskAndCostAndInstantiatedModules;

class MaskAndCost: public Mask
{
public:
    BittreeTaskTypeAsPartialFunction* now_canvas = nullptr;
    BittreeTaskTypeAsPartialFunction* next_canvas = nullptr;
    CanvasAndBittreeProgram* program = nullptr;
    vector<MaskAndCostAndInstantiatedModules*> local_variety;
    pair<MaskAndCost*, int> best_edge;
    bool mask_cost_defined = false;
    int mask_cost;
    MaskAndCost() = default;
    MaskAndCost(MaskAndCost * to_copy);
    MaskAndCost(int _cost, Bitvector _bitvector);
    MaskAndCost(CanvasAndBittreeProgram* _program);

    void set_mask_cost(float _mask_cost)
    {
        mask_cost_defined = true;
        mask_cost = _mask_cost;
    }

    float get_mask_cost() const {
        assert(mask_cost_defined);
        return mask_cost;
    }

    AutomatonRuleCost get_cost();

    virtual bool operator < (const MaskAndCost & other) const;

    bool operator<(const MaskAndCost* &other) const;

    void set_local_variety(vector<MaskAndCostAndInstantiatedModules*> _local_variety);

    string to_string();

    void set_next_bittree_as_partial(BittreeTaskTypeAsPartialFunction* bittree_as_partial);

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

//
// Created by Kliment Serafimov on 8/10/20.
//

#include "Mask.h"
#include "PartialFunction.h"

MaskAndCost::
MaskAndCost(CanvasAndBittreeProgram* _program): Mask(CanvasAsPartialFunction(_program->get_canvas()).total_function)
{
    program = _program;
    best_edge = make_pair(nullptr, -1);
}

void MaskAndCost::set_now_bittree_as_partial(BittreeTaskTypeAsPartialFunction *_now_canvas) {
    now_canvas = _now_canvas;
}

void MaskAndCost::set_best_edge(MaskAndCost *prev_mask, int edge_id) {
    best_edge = make_pair(prev_mask, edge_id);
}

MaskAndCost::MaskAndCost(MaskAndCost *to_copy) : Mask(to_copy)
{
    local_variety = to_copy->local_variety;
    program = to_copy->program;
    now_canvas = to_copy->now_canvas;
    next_canvas = to_copy->next_canvas;
    best_edge = make_pair(nullptr, -1);
    mask_cost_defined = to_copy->mask_cost_defined;
    mask_cost = to_copy->mask_cost;
}

MaskAndCost::MaskAndCost(int _cost, Bitvector _bitvector)  : Mask(_bitvector)
{
    set_mask_cost(_cost);
    best_edge = make_pair(nullptr, -1);
}

AutomatonRuleCost MaskAndCost::get_cost()
{
    return program->get_cost();
}

bool MaskAndCost::operator<(const MaskAndCost &other) const
{
    if(program == nullptr || other.program == nullptr)
    {
        if(get_mask_cost() == other.get_mask_cost())
        {
            return Mask::operator<(other);
        }
        else
        {
            return get_mask_cost() < other.get_mask_cost();
        }
    }
    else {
        if (program->get_cost() != other.program->get_cost()) {
            return program->get_cost() < other.program->get_cost();
        } else {
            return Mask::operator<(other);
        }
    }
}

bool MaskAndCost::operator<(const MaskAndCost* &other) const
{
    assert(other != nullptr);
    if(program == nullptr || other->program == nullptr)
    {
        if(get_mask_cost() == other->get_mask_cost())
        {
            return Mask::operator<(*other);
        }
        else
        {
            return get_mask_cost() < other->get_mask_cost();
        }
    }
    else {
        if (program->get_cost() != other->program->get_cost()) {
            return program->get_cost() < other->program->get_cost();
        } else {
            return Mask::operator<(*other);
        }
    }
}

void MaskAndCost::set_local_variety(
        vector<MaskAndCostAndInstantiatedModules*> _local_variety)
{
    local_variety = std::move(_local_variety);
}

string MaskAndCost::to_string()
{
    return Mask::to_string();
}

void MaskAndCost::set_next_bittree_as_partial(BittreeTaskTypeAsPartialFunction *bittree_as_partial)
{
    next_canvas = bittree_as_partial;
}
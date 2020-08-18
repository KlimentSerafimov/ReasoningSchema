//
// Created by Kliment Serafimov on 8/18/20.
//

#include "BehaviorToProgram.h"
#include "MaskAndCostAndInstantiatedModules.h"
#include "PartialFunction.h"

void BehaviorToProgram::push_back(CanvasAndBittreeProgram *new_canvas_and_bittree_program)
{
    assert(new_canvas_and_bittree_program != nullptr);
    Bitvector behavior = CanvasAsPartialFunction(new_canvas_and_bittree_program->get_canvas()).total_function;
    if(find(behavior) == end())
    {
        (*this)[behavior] = vector<CanvasAndBittreeProgram*>();
    }

    (*this)[behavior].push_back(new_canvas_and_bittree_program);


//    Bitvector behavior = Bitvector(*new_subdomain_as_node_in_prior_topology);
//    if(find(behavior) == end())
//    {
//        (*this)[behavior] = vector<MaskAndCostAndInstantiatedModules*>();
//    }
//    (*this)[behavior].push_back(new_subdomain_as_node_in_prior_topology);
}

CanvasAndBittreeProgram *BehaviorToProgram::init_iter__all()
{
    assert(!in_iter__all);
    in_iter__all = true;
    at_iterator = begin();
    assert(at_iterator->second.size() >= 1);
    at_program_id = 0;
    assert(at_iterator->second[at_program_id] != nullptr);
    return at_iterator->second[at_program_id];
}

bool BehaviorToProgram::end_iter__all()
{
    assert(in_iter__all);
    if(at_iterator == end())
    {
        in_iter__all = false;
        return true;
    }
    return false;
}

CanvasAndBittreeProgram *BehaviorToProgram::get_next__all()
{
    assert(in_iter__all);
    if(at_iterator->second.size() == at_program_id+1)
    {
        at_iterator++;
        at_program_id = 0;
    }
    else
    {
        at_program_id++;
    }
    if(at_iterator == end())
    {
        return nullptr;
    }
    else
    {
        assert(at_program_id < (int)at_iterator->second.size());
        assert(at_iterator->second[at_program_id] != nullptr);
        return at_iterator->second[at_program_id];
    }
}


void BehaviorToProgram::plus_equals(BehaviorToProgram *rhs) {
    for(BehaviorToProgramIterator at = rhs->init_iter__iterators();
        !rhs->end_iter__iterators();
        at = rhs->get_next__iterators())
    {
        if(find(at->first) == end())
        {
            (*this)[at->first] = vector<CanvasAndBittreeProgram*>();
        }
        for(int i = 0;i<at->second.size();i++)
        {
            (*this)[at->first].push_back(at->second[i]);
        }
    }
}

BehaviorToProgramIterator BehaviorToProgram::init_iter__iterators() {
    assert(!in_iter__iterators);
    in_iter__iterators = true;
    at_iterator = begin();
    return at_iterator;
}

bool BehaviorToProgram::end_iter__iterators() {
    assert(in_iter__iterators);
    if(at_iterator == end())
    {
        in_iter__all = false;
        return true;
    }
    return false;
}

BehaviorToProgramIterator BehaviorToProgram::get_next__iterators() {
    assert(in_iter__iterators);
    at_iterator++;
    return at_iterator;
}


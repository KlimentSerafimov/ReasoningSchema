//
// Created by Kliment Serafimov on 8/18/20.
//

#include "BehaviorToProgram.h"
#include "MaskAndCostAndInstantiatedModules.h"
#include "PartialFunction.h"

void BehaviorToProgram::my_push_back(CanvasAndBittreeProgram *new_canvas_and_bittree_program)
{
    assert(new_canvas_and_bittree_program != nullptr);
    Bitvector behavior = CanvasAsPartialFunction(new_canvas_and_bittree_program->get_canvas()).total_function;
    if(find(behavior) == end())
    {
        (*this)[behavior] = multiset<Pointer<CanvasAndBittreeProgram>>();
    }
    (*this)[behavior].insert(new_canvas_and_bittree_program);

//    Bitvector behavior = Bitvector(*new_subdomain_as_node_in_prior_topology);
//    if(find(behavior) == end())
//    {
//        (*this)[behavior] = vector<MaskAndCostAndInstantiatedModules*>();
//    }
//    (*this)[behavior].my_push_back(new_subdomain_as_node_in_prior_topology);
}

CanvasAndBittreeProgram *BehaviorToProgram::init_iter__all()
{
    assert(!in_iter__all);
    in_iter__all = true;
    at_iterator = begin();
    assert(at_iterator->second.size() >= 1);
    at_set_iterator = at_iterator->second.begin();
    return at_set_iterator->get_pointer();
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
    at_set_iterator++;
    if(at_iterator->second.end() == at_set_iterator)
    {
        at_iterator++;
        if(at_iterator != end())
        {
            assert(at_iterator->second.size() >= 1);
            at_set_iterator = at_iterator->second.begin();
        }
    }
    if(at_iterator == end())
    {
        return nullptr;
    }
    else
    {
        return at_set_iterator->get_pointer();
    }
}


void BehaviorToProgram::plus_equals(BehaviorToProgram *rhs) {
    for(auto at = rhs->init_iter__iterators();  !rhs->end_iter__iterators(); at = rhs->get_next__iterators())
    {
        if(find(at->first) == end())
        {
            (*this)[at->first] = multiset<Pointer<CanvasAndBittreeProgram> >();
        }
        for(auto it = at->second.begin();it!=at->second.end();it++)
        {
            (*this)[at->first].insert(*it);
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
        in_iter__iterators = false;
        return true;
    }
    return false;
}

BehaviorToProgramIterator BehaviorToProgram::get_next__iterators() {
    assert(in_iter__iterators);
    at_iterator++;
    return at_iterator;
}


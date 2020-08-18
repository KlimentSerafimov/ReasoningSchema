//
// Created by Kliment Serafimov on 8/15/20.
//

#ifndef COMPACTPOSET_MASKANDCOSTANDINSTANTIATEDMODULES_H
#define COMPACTPOSET_MASKANDCOSTANDINSTANTIATEDMODULES_H

#include "vector"
#include "Mask.h"
#include "Prior.h"

class Module;
class HeuristicScore;

class MaskAndCostAndInstantiatedModules: public MaskAndCost
{
    vector<Module*> modules; //instantiated_modules_per_mask;
    bool unseen_outputs_defined = false;
    Bitvector unseen_outputs;
public:
    MaskAndCostAndInstantiatedModules(MaskAndCost* mask_and_cost): MaskAndCost(mask_and_cost) {}
    MaskAndCostAndInstantiatedModules(CanvasAndBittreeProgram* _program): MaskAndCost(_program) {}
    void add_unseen_output(Bitvector new_unseen_output);
    void push_back(Module* new_module);
    HeuristicScore get_most_receint_heuristic();
    bool has_most_receint_heuristic();
    Bitvector get_unseen_outputs();
    void reset_unseen_output();

};

class MaskAndNextPrior
{
public:
    MaskAndCostAndInstantiatedModules* mask;
    Prior* next_prior;
    explicit MaskAndNextPrior(MaskAndCostAndInstantiatedModules* mask);
    MaskAndNextPrior(MaskAndCostAndInstantiatedModules* mask, Prior* next_prior);
};

class Pointer_MaskAndCostAndInstantiatedModules
{
    MaskAndCostAndInstantiatedModules* pointer = nullptr;
public:
    Pointer_MaskAndCostAndInstantiatedModules(MaskAndCostAndInstantiatedModules* _pointer)
    {
        pointer = _pointer;
    }
    bool operator < (const Pointer_MaskAndCostAndInstantiatedModules& other) const
    {
        assert(pointer != nullptr);
        assert(other.pointer != nullptr);
        return pointer->operator<(other.pointer);
    }
    MaskAndCostAndInstantiatedModules* get_pointer()
    {
        assert(pointer != nullptr);
        return pointer;
    }
};



#endif //COMPACTPOSET_MASKANDCOSTANDINSTANTIATEDMODULES_H

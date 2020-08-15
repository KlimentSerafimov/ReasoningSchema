//
// Created by Kliment Serafimov on 8/15/20.
//

#ifndef COMPACTPOSET_MASKANDCOSTANDINSTANTIATEDMODULES_H
#define COMPACTPOSET_MASKANDCOSTANDINSTANTIATEDMODULES_H

#include "vector"
#include "Mask.h"

class Module;

class MaskAndCostAndInstantiatedModules: public MaskAndCost
{
    vector<Module*> instantiated_modules_per_mask;
public:
    MaskAndCostAndInstantiatedModules(MaskAndCost* mask_and_cost): MaskAndCost(mask_and_cost)
    {

    }
    void push_back(Module* new_module);
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

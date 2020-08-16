//
// Created by Kliment Serafimov on 8/15/20.
//

#include "MaskAndCostAndInstantiatedModules.h"
#include "Module.h"

void MaskAndCostAndInstantiatedModules::push_back(Module *new_module) {
    unseen_outputs = Bitvector(all_zeroes, get_size());
    unseen_outputs_defined = true;
    modules.push_back(new_module);
}

HeuristicScore MaskAndCostAndInstantiatedModules::get_most_receint_heuristic() {
    assert(modules.size() >= 1);
    return modules[modules.size()-1]->get_heuristic_score();
}

bool MaskAndCostAndInstantiatedModules::has_most_receint_heuristic() {
    return modules.size() >= 1 && modules[modules.size()-1]->get_heuristic_score().defined;
}

void MaskAndCostAndInstantiatedModules::add_unseen_output(Bitvector new_unseen_output) {
    if(!unseen_outputs_defined)
    {
        unseen_outputs = new_unseen_output;
    }
    else
    {
        unseen_outputs_defined = true;
        unseen_outputs |= new_unseen_output;
    }
}

Bitvector MaskAndCostAndInstantiatedModules::get_unseen_outputs() {
    assert(unseen_outputs_defined);
    return unseen_outputs;
}

void MaskAndCostAndInstantiatedModules::reset_unseen_output() {
    unseen_outputs = Bitvector(all_ones, get_size());
    unseen_outputs_defined = true;
}

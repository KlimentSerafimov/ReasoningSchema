//
// Created by Kliment Serafimov on 1/8/20.
//

#ifndef COMPACTPOSET_BITVECTORTASKS_H
#define COMPACTPOSET_BITVECTORTASKS_H

#include "PartialFunction.h"

class BitvectorTasks {

    int num_inputs;

    vector<PartialFunction> ordering_over_boolean_functions;
    void populate_ordering_over_boolean_functions_with_small_sum();
    void populate_ordering_over_boolean_functions_with_bitwise_and();


};


#endif //COMPACTPOSET_BITVECTORTASKS_H

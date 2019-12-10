//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "PartialFunction.h"
#include <iostream>
#include "util.h"

PartialFunction::PartialFunction() = default;

PartialFunction::PartialFunction(int _num_inputs, int _total_function, int _partition) {
    num_inputs = _num_inputs;
    function_size = (1<<num_inputs);
    total_function = _total_function;
    if(_partition == -1)
    {
        _partition = (1<<function_size)-1;
    }
    partition = _partition;
}

string PartialFunction::to_string() {
    string ret;
    for(int i = function_size-1;i>=0;i--)
    {
        if(get_bit(partition, i))
        {
            ret+='0'+get_bit(total_function, i);
        }
        else
        {
            ret+="_";
        }
    }
    return ret;
}

bool PartialFunction::is_generalization_of(PartialFunction other_partial_function) {
    assert(num_inputs == other_partial_function.num_inputs);
    if((partition & other_partial_function.partition) == other_partial_function.partition) {
//    assert(partition == (1<<(1<<num_inputs))-1);
        bool ret = ((total_function & other_partial_function.partition) -
                    (other_partial_function.total_function & other_partial_function.partition) == 0);
        return ret;
    } else
    {
        return false;
    }
}

PartialFunction PartialFunction::get_composition(PartialFunction other) {
    assert(num_inputs == other.num_inputs);

    int common_partition = partition & other.partition;

    assert((total_function & common_partition) - (other.total_function & common_partition) == 0);

    int this_part = total_function & ((1<<function_size) - 1 - other.partition);
    int other_part = other.total_function & ((1<<function_size) - 1 - partition);
    int common_part = total_function & common_partition;

    return PartialFunction(num_inputs, this_part|common_part|other_part, partition|other.partition);
}

int PartialFunction::has_output(int idx) {
    return (partition & (1<<idx)) != 0;
}

int PartialFunction::get_output(int idx) {
    return (total_function & (1<<idx)) != 0;
}

void PartialFunction::apply_intersection(PartialFunction other) {

    partition &= other.partition;
    partition &= (1<<function_size) - 1 - ((total_function & partition) ^ (other.total_function & partition));

    assert((partition & total_function) == (partition & other.total_function));
}

int PartialFunction::partition_size() {
    return __builtin_popcount(partition);
}

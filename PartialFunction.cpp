//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "PartialFunction.h"
#include <iostream>
#include "util.h"

PartialFunction::PartialFunction() = default;

PartialFunction::PartialFunction(int _num_inputs, int _total_function, int _partition) {
    function_size = _num_inputs;
    total_function = _total_function;
    if(_partition == -1)
    {
        _partition = (1<<function_size)-1;
    }
    partition = (unsigned int) _partition;
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

bool PartialFunction::is_contained_in(PartialFunction other_partial_function) {
    assert(function_size == other_partial_function.function_size);
    if((partition & other_partial_function.partition) == other_partial_function.partition) {
//    assert(partition == (1<<(1<<function_size))-1);
        bool ret = ((total_function & other_partial_function.partition) -
                    (other_partial_function.total_function & other_partial_function.partition) == 0);
        return ret;
    } else
    {
        return false;
    }
}

PartialFunction PartialFunction::get_composition(PartialFunction other) {
    assert(function_size == other.function_size);

    int common_partition = partition & other.partition;

    assert((total_function & common_partition) - (other.total_function & common_partition) == 0);

    int this_part = total_function & ((1<<function_size) - 1 - other.partition);
    int other_part = other.total_function & ((1<<function_size) - 1 - partition);
    int common_part = total_function & common_partition;

    return PartialFunction(function_size, this_part | common_part | other_part, partition | other.partition);
}

int PartialFunction::has_output(int idx) {
    return (partition & (1<<idx)) != 0;
}

int PartialFunction::get_output(int idx) {
    return (total_function & (1<<idx)) != 0;
}

void PartialFunction::apply_common_partition(PartialFunction other) {

    partition &= other.partition;
    partition &= (1<<function_size) - 1 - ((total_function & partition) ^ (other.total_function & partition));

    assert((partition & total_function) == (partition & other.total_function));
}

int PartialFunction::partition_size() {
    return __builtin_popcount(partition);
}

void PartialFunction::set_bit(int idx, int value) {
    if(!get_bit(partition, idx))
    {
        partition |= (1 << idx);
    }
    total_function &= (1<<function_size)-1-(1<<idx);
    total_function |= (value << idx);
    assert(get_bit(total_function, idx) == value);
}

void PartialFunction::clear_bit(int idx) {
    assert(get_bit(partition, idx) == 1);
    partition -= (1<<idx);
}

bool PartialFunction::empty() {
    return partition == 0;
}

bool PartialFunction::has_empty_intersection_with(PartialFunction other) {
    int common_partition = partition & other.partition;
    int difference_mask = ((total_function & common_partition) ^ (other.total_function & common_partition));
    return difference_mask != 0;
}

void PartialFunction::append_difference_with(PartialFunction other, vector<PartialFunction> &to_append_to) {

    if (!has_empty_intersection_with(other)) {
        int template_for_output__partition = partition;
        int template_for_output__function = total_function & partition;

        unsigned int other_contains_but_this_doesnt = other.partition - (other.partition & partition);

        while(other_contains_but_this_doesnt != 0)
        {
            int idx_of_first_one = num_trailing_zeroes(other_contains_but_this_doesnt);

            template_for_output__partition |= (1<<idx_of_first_one);

            to_append_to.push_back(
                    PartialFunction(
                            function_size,
                            template_for_output__function | ((1-get_bit(other.total_function, idx_of_first_one)) << idx_of_first_one),
                            template_for_output__partition
                            )
                    );

            template_for_output__function |= (get_bit(other.total_function, idx_of_first_one) << idx_of_first_one);

            other_contains_but_this_doesnt -= (1<<idx_of_first_one);
        }
    }
    else
    {
        to_append_to.push_back(PartialFunction(function_size, total_function, partition));
    }
}

void PartialFunction::append_intersection_with(PartialFunction other, vector<PartialFunction> &to_append_to) {
    if(!has_empty_intersection_with(other))
    {
        to_append_to.push_back(
                PartialFunction(
                        function_size,
                        (total_function & partition) | (other.total_function & other.partition),
                        partition | other.partition
                        )
                );
    }
}

bool PartialFunction::full()
{
    return partition == (1<<function_size)-1;
}

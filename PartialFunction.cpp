//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "PartialFunction.h"

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
    for(int i = 0;i<function_size;i++)
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
    assert(partition == (1<<(1<<num_inputs))-1);
    bool ret = ((total_function & other_partial_function.partition) -
            (other_partial_function.total_function & other_partial_function.partition) == 0);
    return ret;

}

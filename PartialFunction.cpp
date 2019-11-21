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

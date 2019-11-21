//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "MetaExample.h"
#include "util.h"

MetaExample::MetaExample(
        int _num_inputs, int _total_function, int _partition) {
    partial_function = PartialFunction(_num_inputs, _total_function, _partition);
    total_function = PartialFunction(_num_inputs, _total_function, (1<<partial_function.function_size)-1);
}

MetaExample::MetaExample(PartialFunction _partial_function){
    partial_function = _partial_function;
    total_function = PartialFunction(
            partial_function.num_inputs, partial_function.total_function,  (1<<partial_function.function_size)-1);
}


int MetaExample::get_compact_partial_outputs()
{
    int compact_partial_outputs = 0;
    for(int i = 0, j = 0;i<partial_function.function_size; i++) {
        if (get_bit(partial_function.partition, i)) {
            compact_partial_outputs += (1 << j) * get_bit(partial_function.total_function, i);
            j++;
        }
    }
    return compact_partial_outputs;
}

int MetaExample::get_compact_hidden_outputs() {
    int compact_hidden_outputs = 0;
    for(int i = 0, j = 0;i<partial_function.function_size; i++) {
        if (!get_bit(partial_function.partition, i)) {
            compact_hidden_outputs += (1 << j) * get_bit(partial_function.total_function, i);
            j++;
        }
    }
    return compact_hidden_outputs;
}


string MetaExample::linear_string() {
    return linear_string(0);
}

string MetaExample::linear_string(int tab)
{
    string ret;
    for(int i = 0;i<tab;i++)
    {
        ret+="\t";
    }
    ret += "{";
    ret += "(";
    bool is_first = true;
    for(int i = 0;i<partial_function.function_size;i++)
    {
        if(get_bit(partial_function.partition, i))
        {
            if(!is_first)
            {
                ret += " && ";
            }
            else
            {
                is_first = false;
            }
            ret += bitvector_to_str(i, partial_function.num_inputs);
            ret += "->";
            ret += bitvector_to_str(get_bit(partial_function.total_function, i), 1);

        }
    }
    ret += ") ";
    ret += "-->";
    ret += " (";
    is_first = true;
    for(int i = 0;i<partial_function.function_size;i++)
    {
        if(!get_bit(partial_function.partition, i))
        {
            if(!is_first)
            {
                ret += " && ";
            }
            else
            {
                is_first = false;
            }
            ret += bitvector_to_str(i, partial_function.num_inputs);
            ret += "->";
            ret += bitvector_to_str(get_bit(partial_function.total_function, i), 1);
        }
    }
    ret += ")";
    ret += "}";
    return ret;
}

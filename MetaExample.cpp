//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "MetaExample.h"
#include "util.h"
#include <iostream>

MetaExample::MetaExample(
        int _num_inputs, int _total_function, int _partition) {
    partial_function = PartialFunction(_num_inputs, _total_function, _partition);
    generalization = PartialFunction(_num_inputs, _total_function, -1);
}

MetaExample::MetaExample(int _num_inputs, int _total_function, int _partition, int generalization_partition) {

    partial_function = PartialFunction(_num_inputs, _total_function, _partition);
    generalization = PartialFunction(_num_inputs, _total_function, generalization_partition);
}


MetaExample::MetaExample(PartialFunction _partial_function){
    partial_function = _partial_function;
    generalization = PartialFunction(
            partial_function.num_inputs, partial_function.total_function, -1);
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
//    ret += "{";
    ret += "(";
    ret += partial_function.to_string();
    ret += " -> ";
    ret += generalization.to_string();
    ret += ")";
//    ret += "}";
    return ret;
}

void MetaExample::print()
{
    cout << linear_string() << endl;
}

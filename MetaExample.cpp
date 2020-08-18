//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "MaskAndCostAndInstantiatedModules.h"
#include "MetaExample.h"
#include "util.h"
#include "Prior.h"
#include <iostream>

MetaExample::MetaExample(
        int _function_size, Bitvector _total_function, Bitvector _partition) {
    partial_function = PartialFunction(_function_size, _total_function, _partition);
    generalization = PartialFunction(_function_size, _total_function, Bitvector(all_ones, _function_size));
}

MetaExample::MetaExample(int _function_size, Bitvector _total_function, Bitvector _partition, Bitvector generalization_partition) {

    partial_function = PartialFunction(_function_size, _total_function, _partition);
    generalization = PartialFunction(_function_size, _total_function, generalization_partition);
}


MetaExample::MetaExample(PartialFunction _partial_function){
    partial_function = _partial_function;
    generalization = PartialFunction(
            partial_function.function_size, partial_function.total_function, Bitvector(all_ones, partial_function.function_size));
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


string MetaExample::to_string() {
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
//    ret += "(";
//    ret += "   ";
    ret += partial_function.to_string();
    ret += " -> ";
    ret += generalization.to_string();
//    ret += ")";
//    ret += "}";
    return ret;
}

void MetaExample::print()
{
    cout << to_string() << endl;
}

MetaExample::MetaExample(int _function_size, Bitvector _total_function, Bitvector _partition, Bitvector generalization_partition, int _idx) {
    partial_function = PartialFunction(_function_size, _total_function, _partition);
    generalization = PartialFunction(_function_size, _total_function, generalization_partition);
    idx = _idx;
}

MetaExample::MetaExample(int _function_size, Bitvector _total_function, Bitvector _partition, Bitvector generalization_partition, int _idx, bool _active) {
    partial_function = PartialFunction(_function_size, _total_function, _partition);
    generalization = PartialFunction(_function_size, _total_function, generalization_partition);
    idx = _idx;
    active = _active;
}

bool MetaExample::is_consistent_with(MetaExample other) {
    assert(generalization.partition == other.generalization.partition);

//    cout << to_string() <<" " << other.to_string() << endl;

    if(other.generalization.is_contained_in(partial_function) &&
            generalization.is_contained_in(other.partial_function))
    {
        bool ret =
            (other.generalization.total_function & other.generalization.partition) ==
            (generalization.total_function & generalization.partition);
        return ret;
    }

    return true;
}

//MetaExample::MetaExample(PartialFunction _partial_function, PartialFunction _generalization, int _idx) {
//    assert(_generalization.is_contained_in(_partial_function));
//    partial_function = _partial_function;
//    generalization = _generalization;
//    idx = _idx;
//}

MetaExample::MetaExample(BittreeTaskTypeAsPartialFunction _partial_function, BittreeTaskTypeAsPartialFunction _generalization, int _idx) {
    assert(_generalization.is_contained_in(_partial_function));
    partial_function = _partial_function;
    generalization = _generalization;
    idx = _idx;
}

Bitvector MetaExample::get_generalization_mask() {
    assert((partial_function.partition & generalization.partition) == partial_function.partition);
    return generalization.partition;
//    Bitvector input_mask = partial_function.partition & generalization.partition;
//    Bitvector output_mask = generalization.partition ^ input_mask;
//    return output_mask;
}

Bitvector MetaExample::get_output_mask()
{
    Bitvector input_mask = partial_function.partition & generalization.partition;
    Bitvector output_mask = generalization.partition ^ input_mask;
    return output_mask;
}

int get_num_missing_bits(vector<MetaExample> meta_examples)
{
    int ret = 0;
    for(int i = 0; i<meta_examples.size();i++)
    {
        ret += meta_examples[i].generalization.partition.count() -
               meta_examples[i].partial_function.partition.count();
    }
    return ret;
}



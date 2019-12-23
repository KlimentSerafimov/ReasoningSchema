//
// Created by Kliment Serafimov on 2019-12-23.
//

#ifndef COMPACTPOSET_MODULE_H
#define COMPACTPOSET_MODULE_H

#include <vector>
#include "CompactPoset.h"
#include "MetaExample.h"

class Module
{
public:
    int function_size;
    int subdomain_mask;

    Module* parent_module;

    vector<MetaExample> module_meta_examples;
    vector<vector<int> > equivalent_ids;

    vector<MetaExample> meta_examples_after_query;
    CompactPoset* compact_poset;
    int intermediate_num_missing_bits;

    vector<pair<int, Module*> > repeats;
    vector<int> num_missing_bits_after_repeat;

    int num_missing_bits;
    vector<int> necessary_meta_example_ids;
    vector<vector<vector<int> > > covered;

    string print_module_sketch(time_t delta_time);

    string covered_to_string(vector<MetaExample> init_meta_examples);
};

#endif //COMPACTPOSET_MODULE_H

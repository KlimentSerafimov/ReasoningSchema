//
// Created by Kliment Serafimov on 2019-12-23.
//

#ifndef COMPACTPOSET_MODULE_H
#define COMPACTPOSET_MODULE_H

#include <vector>
#include "CompactPoset.h"
#include "MetaExample.h"

class MinimalFactoringSchema;

class Module
{
public:
    int function_size;
    int subdomain_mask;

    Module* parent_module;

    MinimalFactoringSchema* parent_minimal_factoring_schema;

    vector<MetaExample> module_meta_examples;
    vector<vector<int> > equivalent_ids;

    CompactPoset* compact_poset = NULL;
    int intermediate_num_missing_bits;
    vector<MetaExample> meta_examples_after_query;

    vector<Module*> repeats_module_pointers;
    vector<int> repeats_module_ids;
    vector<int> num_missing_bits_per_repeat;

    int num_missing_bits;

    vector<vector<pair<int, pair<int, int> > > > count_occurences;
    vector<vector<vector<int> > > covering;
    vector<int> necessary_meta_example_ids;


    string print_module_sketch(time_t delta_time);

    string covered_to_string(vector<MetaExample> init_meta_examples);

    ///----------------MUTLI-LANGUAGE----------------------

//    int function_size;
//    int subdomain_mask;

//    Module* parent_module;

    vector<vector<MetaExample> > module_meta_examples_per_language;
    vector<vector<vector<int> > > equivalent_ids_per_language;

    vector<CompactPoset*> compact_poset_per_language;
    vector<int> intermediate_num_missing_bits_per_language;
    vector<vector<MetaExample> > meta_examples_after_query_per_language;

//    vector<Module*> repeats_module_pointers;
//    vector<int> repeats_module_ids;
    vector<vector<int> > num_missing_bits_per_repeat_per_language;

    vector<int> num_missing_bits_per_language;

//    vector<vector<pair<int, pair<int, int> > > > count_occurences;
//    vector<vector<vector<int> > > covering;
//    vector<int> necessary_meta_example_ids;

    Module() = default;

    Module(MinimalFactoringSchema* _minimal_factoring_schema)
    {
        parent_minimal_factoring_schema = _minimal_factoring_schema;
    }
};

#endif //COMPACTPOSET_MODULE_H

//
// Created by Kliment Serafimov on 2019-12-23.
//

#ifndef COMPACTPOSET_MODULE_H
#define COMPACTPOSET_MODULE_H

#include <vector>
#include "CompactPoset.h"
#include "MetaExample.h"


class HeuristicScore
{
public:
    bool defined = false;
    int num_input_bits;
    double ratio_delta_meta_examples_per_new_bit;

    bool operator < (const HeuristicScore& other) const
    {
        if(defined && other.defined) {
//            if (num_input_bits == other.num_input_bits) {
            return ratio_delta_meta_examples_per_new_bit > other.ratio_delta_meta_examples_per_new_bit;
//            } else {
//                return num_input_bits < other.num_input_bits;
//            }
        }
        else
        {
            if(defined)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    HeuristicScore(int _num_input_bits, double _ratio_delta_meta_examples_per_new_bit) {
        num_input_bits = _num_input_bits;
        ratio_delta_meta_examples_per_new_bit = _ratio_delta_meta_examples_per_new_bit;
        defined = true;
    }

    HeuristicScore() {}

    string to_string()
    {
        if(defined) {
            return std::to_string(num_input_bits) + " " + std::to_string(ratio_delta_meta_examples_per_new_bit);
        }
        else
        {
            return "inf";
        }
    }
};


class MinimalFactoringSchema;

class Module
{
public:
    int function_size;
    Bitvector subdomain_mask;
    HeuristicScore heuristic_score;

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

    Module() = default;

    Module(MinimalFactoringSchema* _minimal_factoring_schema)
    {
        parent_minimal_factoring_schema = _minimal_factoring_schema;
    }
};

#endif //COMPACTPOSET_MODULE_H

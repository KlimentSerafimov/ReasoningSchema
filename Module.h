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
    double heuristic;



    bool operator == (const HeuristicScore& other) const
    {
        return defined == other.defined && num_input_bits == other.num_input_bits && heuristic == other.heuristic;
    }
    bool operator < (const HeuristicScore& other) const
    {
        if(defined && other.defined) {
            if(heuristic == other.heuristic)
            {
                return num_input_bits < other.num_input_bits;
            }
            return heuristic > other.heuristic;
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
        heuristic = _ratio_delta_meta_examples_per_new_bit;
        defined = true;
    }

    HeuristicScore() {}

    string to_string()
    {
        if(defined) {
            return std::to_string(num_input_bits) + " " + std::to_string(heuristic);
        }
        else
        {
            return "inf";
        }
    }
};


class ReasoningSchemaOptimizer;

class Module
{
    bool defined_heuristic_score = false;
    HeuristicScore heuristic_score;
public:
    int function_size;
    MaskAndCostAndInstantiatedModules* subdomain_mask;

    Module* parent_module;

    ReasoningSchemaOptimizer* parent_minimal_factoring_schema;

    vector<MetaExample> module_meta_examples;
    vector<vector<int> > equivalent_ids;

    CompactPoset* compact_poset = nullptr;
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

    Module(ReasoningSchemaOptimizer* _minimal_factoring_schema)
    {
        parent_minimal_factoring_schema = _minimal_factoring_schema;
    }

    string subdomain_mask_to_string(MaskAndCost subdomain_mask);

    string meta_example_to_string(MetaExample meta_example);

    Bitvector get_generalization_mask()
    {
        assert(compact_poset != nullptr);
        Bitvector ret = compact_poset->get_generalization_mask();
        for(int i = 0;i<repeats_module_pointers.size();i++)
        {
            ret |= repeats_module_pointers[i]->get_generalization_mask();
        }
        return ret;
    }
    HeuristicScore get_heuristic_score()
    {
        assert(defined_heuristic_score);
        return heuristic_score;
    }
    void set_heuristic_score(HeuristicScore _heuristic_score);
};

#endif //COMPACTPOSET_MODULE_H

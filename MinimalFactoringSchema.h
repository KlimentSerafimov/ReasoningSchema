//
// Created by Kliment Serafimov on 2019-12-18.
//

#ifndef COMPACTPOSET_MINIMALFACTORINGSCHEMA_H
#define COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

#include "Module.h"

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

public:
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

class MinimalFactoringSchema
{
    MinimalFactoringSchema* parent_pointer = NULL;
    int function_size;

    vector<Bitvector> masks;
    int moule_id;

    time_t init_time;
    time_t local_time;

    vector<MetaExample> meta_examples;
    int init_num_missing_bits;

    MinimalFactoringSchema* next = nullptr;

    void calc_function_size();

    bool test_compact_poset_for_consistency_with_all_meta_examples(Bitvector subdomain_mask, CompactPoset *compact_poset);

    void prune_globally_inconsistent_meta_examples(Bitvector subdomain_mask, CompactPoset *compact_poset);

    int get_meta_edge_id_to_remove(CompactPoset* compact_poset, Bitvector subdomain_mask, int special_meta_example_id);

    HeuristicScore calculate_heuristic(Module* module);

    void repeat_apply_parents(Module *module);

    void calc_masks(int set_init_masks_size);

    void calc_module(Bitvector subdomain_mask, Module *module);

    void main__minimal_factoring_schema(vector<MetaExample> _meta_examples);

public:

    Module best_module;

    MinimalFactoringSchema* root_pointer;

    MinimalFactoringSchema(vector<MetaExample> _meta_examples, MinimalFactoringSchema *_parent_pointer);

    MinimalFactoringSchema(vector<MetaExample> _meta_examples, string ordering_name, bool skip);

    MinimalFactoringSchema(vector<MetaExample> _meta_examples, string ordering_name, vector<Bitvector> mask);

    vector<MetaExample> get_necessary_meta_examples(bool print);

    PartialFunction query(PartialFunction partial_function);

    PartialFunction query(PartialFunction partial_function, Module* stop_at_pointer);

    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > record_trace_of_query(PartialFunction partial_function);

    void record_trace_of_query(PartialFunction partial_function, Module *pointer_to_stop,
                                   vector<PartialFunction> &trace, vector<int> &active_operators,
                                   vector<PartialFunction> &active_trace);

};

vector<MetaExample> get_meta_examples_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
        int subdomain_mask, vector<MetaExample> meta_examples);

vector<MetaExample> get_meta_examples_after_query(Bitvector subdomain_mask, CompactPoset *compact_poset, vector<MetaExample> meta_examples,
                                                  bool print, bool query_only_active, bool carry_over_active);



#endif //COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

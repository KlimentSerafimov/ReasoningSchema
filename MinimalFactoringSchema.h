//
// Created by Kliment Serafimov on 2019-12-18.
//

#ifndef COMPACTPOSET_MINIMALFACTORINGSCHEMA_H
#define COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

#include "Module.h"

class MinimalFactoringSchema
{
    MinimalFactoringSchema* parent_pointer;
    int num_inputs;
    int function_size;

    int moule_id;

    time_t init_time;
    time_t local_time;

    vector<MetaExample> meta_examples;
    int init_num_missing_bits;

    Module best_module;
    MinimalFactoringSchema* next = nullptr;

    bool test_compact_poset_for_consistency_with_all_meta_examples(int subdomain_mask, CompactPoset *compact_poset);

    void prune_globally_inconsistent_meta_examples(int subdomain_mask, CompactPoset *compact_poset);

    int get_meta_edge_id_to_remove(CompactPoset* compact_poset, int subdomain_mask, int special_meta_example_id);

    double calculate_heuristic(Module* module);

    void repeat_apply_parents(Module *module);

    vector<int> get_masks(int set_init_masks_size);

    void calc_module(int subdomain_mask, Module *module);

    void main__minimal_factoring_schema(int _num_inputs, vector<MetaExample> _meta_examples);

    //--------- MULTI LANGUAGE

//    MinimalFactoringSchema* root_pointer;
//    MinimalFactoringSchema* parent_pointer;
//    int num_inputs;
//    int function_size;

//    time_t init_time;
//    time_t local_time;

    vector<vector<MetaExample> > meta_examples_per_language;
    vector<int> init_num_missing_bits_per_language;

//    Module best_module;
//    MinimalFactoringSchema* next = nullptr;


//    vector<MetaExample> get_meta_examples_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
//            int subdomain_mask);
//
//    bool test_compact_poset_for_consistency_with_all_meta_examples(int subdomain_mask, CompactPoset *compact_poset);
//
//    void prune_globally_inconsistent_meta_examples(int subdomain_mask, CompactPoset *compact_poset);
//
//    int get_meta_edge_id_to_remove(CompactPoset* compact_poset, int subdomain_mask, int special_meta_example_id);
//
//    double calculate_heuristic(Module* module);
//
//    void repeat_apply_parents(Module *module);
//
//    vector<int> get_masks(int set_init_masks_size);

    void calc_multi_language_module(int subdomain_mask, Module *module);

//    void main__minimal_factoring_schema(int _num_inputs, vector<MetaExample> _meta_examples);

public:

    MinimalFactoringSchema* root_pointer;

    MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples, MinimalFactoringSchema *parent_pointer);

    MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples,
                               string ordering_name, bool skip);

    MinimalFactoringSchema(int _num_inputs, vector<vector<MetaExample> > _meta_examples_per_language, string ordering_name);

    vector<MetaExample> get_necessary_meta_examples();

    PartialFunction query(PartialFunction partial_function);

    PartialFunction query(PartialFunction partial_function, Module* stop_at_pointer);

    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > record_trace_of_query(PartialFunction partial_function);

    void record_trace_of_query(PartialFunction partial_function, Module *pointer_to_stop,
                                   vector<PartialFunction> &trace, vector<int> &active_operators,
                                   vector<PartialFunction> &active_trace);

};

vector<MetaExample> get_meta_examples_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
        int subdomain_mask, vector<MetaExample> meta_examples);

vector<MetaExample> get_meta_examples_after_query(int subdomain_mask, CompactPoset *compact_poset, vector<MetaExample> meta_examples,
                                                  bool print, bool query_only_active, bool carry_over_active);



#endif //COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

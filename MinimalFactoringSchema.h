//
// Created by Kliment Serafimov on 2019-12-18.
//

#ifndef COMPACTPOSET_MINIMALFACTORINGSCHEMA_H
#define COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

#include "Module.h"

class MinimalFactoringSchema
{

    MinimalFactoringSchema* root_pointer;
    MinimalFactoringSchema* parent_pointer;
    int num_inputs;
    int function_size;

    time_t init_time;
    time_t local_time;

    vector<MetaExample> meta_examples;
    int init_num_missing_bits;

//    int best_mask_id;
//    int best_subdomain_mask;
//    int best_necessary_num_meta_examples;
//    int local_after_num_missig_bits;

    Module best_module;

    vector<MetaExample> get_meta_example_ids_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
            int subdomain_mask);

    bool test_compact_poset_for_consistency_with_all_meta_examples(int subdomain_mask, CompactPoset *compact_poset);

    void prune_globally_inconsistent_meta_examples(int subdomain_mask, CompactPoset *compact_poset);

    int get_meta_edge_id_to_remove(CompactPoset* compact_poset, int subdomain_mask, int special_meta_example_id);

    double calculate_heuristic(Module* module);

    void repeat_apply_parents(Module *module);

    vector<int> get_masks(int set_init_masks_size);

    void calc_module(int subdomain_mask, Module* module, bool print_repeats);

public:

    MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples,
                           MinimalFactoringSchema *parent_pointer);
    MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples,
                               string ordering_name);
    void main__minimal_factoring_schema(int _num_inputs, vector<MetaExample> _meta_examples);
};


vector<MetaExample> get_meta_examples_after_query(int subdomain_mask, CompactPoset *compact_poset, vector<MetaExample> meta_examples,
                                                  bool print, bool query_only_active, bool carry_over_active);



#endif //COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

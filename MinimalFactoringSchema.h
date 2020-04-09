//
// Created by Kliment Serafimov on 2019-12-18.
//

#ifndef COMPACTPOSET_MINIMALFACTORINGSCHEMA_H
#define COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

#include "Module.h"

class MinimalFactoringSchema
{
    MinimalFactoringSchema* parent_pointer = NULL;
    int function_size;

    time_t init_time;
    time_t local_time;

    vector<MetaExample> meta_examples;
    int init_num_missing_bits;

    vector<Bitvector> masks;
    int module_id;
    vector<pair<HeuristicScore, int> > mask_ids_by_heuristic;
    vector<HeuristicScore> heuristic_score_by_mask_id;

    MinimalFactoringSchema* next = nullptr;

    void calc_function_size();

    bool test_compact_poset_for_consistency_with_all_meta_examples(Bitvector subdomain_mask, CompactPoset *compact_poset);

    HeuristicScore calculate_heuristic(Module* module);

    void repeat_apply_parents(Module *module);

    void calc_masks(int set_init_masks_size);

    void calc_module(Bitvector subdomain_mask, Module *module);

    void main__minimal_factoring_schema(vector<MetaExample> _meta_examples);

    bool skip_mask(Bitvector subdomain_mask);

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

#endif //COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

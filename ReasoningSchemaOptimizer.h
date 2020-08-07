//
// Created by Kliment Serafimov on 2019-12-18.
//

#ifndef COMPACTPOSET_REASONINGSCHEMAOPTIMIZER_H
#define COMPACTPOSET_REASONINGSCHEMAOPTIMIZER_H

#include "Module.h"
#include "BitvectorTasks.h"
#include "MetricType.h"

class ReasoningSchemaOptimizer
{
    ReasoningSchemaOptimizer* parent_pointer = NULL;
    int function_size;
    MetricType metric;

    time_t init_time;
    time_t local_time;

    vector<MetaExample> meta_examples;
    int init_num_missing_bits;

    vector<vector<Bitvector> > masks;
    int module_id;
    vector<pair<HeuristicScore, pair<int, int> > > mask_ids_by_heuristic;
    vector<vector<HeuristicScore> > heuristic_score_by_bucket_id_by_mask_id;

    ReasoningSchemaOptimizer* next = nullptr;

    void calc_function_size();

    bool test_compact_poset_for_consistency_with_all_meta_examples(Bitvector subdomain_mask, CompactPoset *compact_poset);

    HeuristicScore calculate_heuristic(Module* module);

    void repeat_apply_parents(Module *module);

    void calc_masks(int set_init_masks_size, int set_end_masks_size);

    void calc_module(Bitvector subdomain_mask, Module *module);

    void main__minimal_factoring_schema(vector<MetaExample> _meta_examples);

    bool skip_mask(Bitvector subdomain_mask);

public:

    Module best_module;

    ReasoningSchemaOptimizer* root_pointer;

    ReasoningSchemaOptimizer(vector<MetaExample> _meta_examples, ReasoningSchemaOptimizer *_parent_pointer);

    ReasoningSchemaOptimizer(vector<MetaExample> _meta_examples, string ordering_name);

    ReasoningSchemaOptimizer(vector<MetaExample> _meta_examples, string ordering_name, vector<vector<Bitvector> > mask,
                             string dir_path, MetricType metric_type);

    vector<MetaExample> get_necessary_meta_examples(bool print);

    vector<Bitvector> get_subdomains();

    PartialFunction query(PartialFunction partial_function);

    PartialFunction query(PartialFunction partial_function, Module* stop_at_pointer);

    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > record_trace_of_query(PartialFunction partial_function);

    void record_trace_of_query(PartialFunction partial_function, Module *pointer_to_stop,
                                   vector<PartialFunction> &trace, vector<int> &active_operators,
                                   vector<PartialFunction> &active_trace);

    BittreeTaskTypeAsPartialFunction *get_copy_of_bottree_task_type();

    string bitvector_to_string__one_line(Bitvector bitvector);

    string partial_function_to_string__one_line(PartialFunction partial_function);

    string meta_example_to_string__one_line(MetaExample meta_example);

    ReasoningSchemaOptimizer();

    vector<Module *> get_modules();
};

#endif //COMPACTPOSET_REASONINGSCHEMAOPTIMIZER_H

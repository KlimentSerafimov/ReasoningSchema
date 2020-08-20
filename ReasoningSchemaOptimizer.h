//
// Created by Kliment Serafimov on 2019-12-18.
//

#ifndef COMPACTPOSET_REASONINGSCHEMAOPTIMIZER_H
#define COMPACTPOSET_REASONINGSCHEMAOPTIMIZER_H

#include "Module.h"
#include "MetricType.h"
#include "MaskAndCostAndInstantiatedModules.h"

class MaskIdAndHeuristicScore
{
    HeuristicScore score;
    pair<int, int> id;

public:
    MaskIdAndHeuristicScore(pair<HeuristicScore, pair<int, int> > score_and_id)
    {
        score = score_and_id.first;
        id = score_and_id.second;
    }

    pair<int, int> get_id() {
        return id;
    }

    HeuristicScore get_score() {
        return score;
    }

    bool operator < (const MaskIdAndHeuristicScore & other) const
    {
        return score < other.score;
    }
};

class ReasoningSchemaOptimizer
{
    ReasoningSchemaOptimizer* parent_pointer = nullptr;
    int function_size;
    MetricType metric;

    string name;
    ofstream* fout_rso;
    string dir_path;
    time_t init_time;
    time_t local_time;

    vector<MetaExample> meta_examples;
    int init_num_missing_bits;

    MaskBuckets masks;
    Prior* prior;
    int module_id;
    vector<MaskIdAndHeuristicScore> mask_ids_by_heuristic;
//    vector<vector<HeuristicScore> > heuristic_score_by_bucket_id_by_mask_id;

    ReasoningSchemaOptimizer* next = nullptr;

    void calc_function_size();

    bool test_compact_poset_for_consistency_with_all_meta_examples(MaskAndCost subdomain_mask, CompactPoset *compact_poset);

    HeuristicScore calculate_heuristic(Module* module);

    void repeat_apply_parents(Module *module);

    void calc_masks();

    void calc_module(MaskAndCostAndInstantiatedModules *subdomain_mask, Module *module);

    void main__minimal_factoring_schema(vector<MetaExample> _meta_examples);


public:

    Module best_module;

    ReasoningSchemaOptimizer* root_pointer;

    ReasoningSchemaOptimizer(vector<MetaExample> _meta_examples, ReasoningSchemaOptimizer *_parent_pointer, Prior * prior);

    ReasoningSchemaOptimizer(vector<MetaExample> _meta_examples, string name, MaskBuckets mask,
                             string dir_path, MetricType metric_type);

    vector<MetaExample> get_necessary_meta_examples(bool print);

    MaskBucket get_subdomains();

    PartialFunction query(PartialFunction partial_function);

    PartialFunction query(PartialFunction partial_function, Module* stop_at_pointer);

    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > record_trace_of_query(PartialFunction partial_function);

    void record_trace_of_query(PartialFunction partial_function, Module *pointer_to_stop,
                                   vector<PartialFunction> &trace, vector<int> &active_operators,
                                   vector<PartialFunction> &active_trace);

    BittreeTaskTypeAsPartialFunction *get_copy_of_bottree_task_type();

    string bitvector_to_string__one_line(MaskAndCost MaskAndCost);

    string partial_function_to_string__one_line(PartialFunction partial_function);

    string meta_example_to_string__one_line(MetaExample meta_example);

    ReasoningSchemaOptimizer();

    vector<Module *> get_modules();

    vector<Module *> get_program_as_vector_of_modules();
};

#endif //COMPACTPOSET_REASONINGSCHEMAOPTIMIZER_H

//
// Created by Kliment Serafimov on 1/8/20.
//

#ifndef COMPACTPOSET_BITVECTORTASKS_H
#define COMPACTPOSET_BITVECTORTASKS_H

#include "ReasoningSchemaOptimizer.h"
#include "IncrementalTypeExpression.h"
#include "MetricType.h"
#include <fstream>

class MetaExample;

static const int max_instance_tree_degree = 8;

class InstanceTree
{
public:
    BittreeTaskType* instance = nullptr;
    Task * task_name = nullptr;

    bool prepared_for_deepening = false;
    BittreeTaskType* superinstance_type = nullptr;

    int num_superinstances = 0;
    BittreeTaskType* superinstances[max_instance_tree_degree];

    bool deepened = false;
    int superinstance_trees_size = 0;
    InstanceTree* superinstance_trees[max_instance_tree_degree];

    InstanceTree() = default;

    InstanceTree(BittreeTaskType* _instance, Task *_task_name);

    void prepare_for_deepening(BittreeInputOutputType* _delta);

    void deepen(BittreeInputOutputType* delta);

    void populate_meta_examples(vector<vector<MetaExample> >& ret, int at_depth, int subtask_depth);

    void vectorize_instance_tree(vector<vector<InstanceTree*> >& ret, int at_depth);
};

class BittreeTaskInstance
{
public:

    BittreeTaskType* initialization;
    BittreeTaskType* delta;

    BittreeTaskType curriculum[5];

    BittreeTaskType* bittree_task_type;

    vector<BittreeTaskType> instances;

    BittreeTaskInstance(BittreeTaskType* _bittree_task_type);
};


class BitvectorTasks {

    Task *task_name;
    int init_iter;
    int num_iter;
    int recursive_rep_set_depth;
    MetricType metric;
    ModeType mode;
    int min_mask_size;
    int max_mask_size;
    int num_prev_subtasks;
    string dir_path;
    bool train_set_minimization;
    int seed_train_set;
    int num_minimization_steps;
    double init_minimization_fraction;
    double end_minimization_fraction;
    AutomatonRuleCost max_automaton_rule_cost;

    InstanceTree instance_tree;

    time_t init_time;
    ofstream summary;
    ofstream summary_with_times;

    int num_inputs;
    int function_size;

    vector<PartialFunction> ordering_over_boolean_functions;
    MetaExample get_meta_example(PartialFunction partial_function);

    void populate_ordering_over_boolean_functions_with_small_sum();
    void populate_ordering_over_boolean_functions_with_bitwise_and();

    vector<vector<vector<MaskAndCost> > >
    masks_generator(int num_subtasks, int max_masks_size, int min_mask_size, int num_first_in_prior, vector<BittreeTaskType*> multi_task_set);

    vector<BittreeTaskType*> get_multi_task_type(IncrementalTypeExpression *type_expression, int init_num_iter);

    void get_meta_examples(IncrementalTypeExpression *type_expression, Task *task_name, int init_num_iter,
                           int subtask_depth, vector<vector<MetaExample> >& ret_meta_examples,
                           vector<vector<InstanceTree*> >& ret_inst_trees);

    void set_up_directory();


    void one_step_of_incremental_meta_generalization(bool is_first, int task_id,
                                                                vector<MetaExample> meta_examples_of_task_id,
                                                                vector<InstanceTree*> instance_subtrees_of_task_id,
                                                                vector<MaskAndCost> &next_subdomains,
                                                                vector<vector<MaskAndCost> > masks_of_task_id,
                                                                BittreeTaskType *task_type,
                                                                BittreeTaskType *next_task_type,
                                                                vector<MaskAndCost> &prev_subdomains,
                                                                vector<MaskAndCost> &new_prev_subdomains,
                                                                vector<MetaExample> prev_train_set,
                                                                vector<InstanceTree*> prev_instance_subtrees,

                                                                vector<MetaExample> & ret_train_meta_exampes,
                                                                vector<InstanceTree*> & ret_train_instnace_subtrees,
                                                                ReasoningSchemaOptimizer* & ret_reasoning_schema);

    void augment_subdomains(vector<MaskAndCost>& subdomains, BittreeTaskType* current_bittree, int num_prev_subtasks, int task_id);

    vector<MaskAndCost> get_next_subdomains(
            MetricType metric, string dir_path, string init_language_name,
            vector<MaskAndCost> & subdomains, BittreeTaskType * current_bittree, BittreeTaskType * next_bittree, int num_prev_subtasks, int task_id);

    void delta_wiring(vector<MaskAndCost> &subdomains, BittreeTaskType *task_type, int task_id,
                      vector<MaskAndCost> &next_subdomains, string init_language_name,
                      BittreeTaskType *next_task_type, vector<MaskAndCost> &prev_subdomains,
                      vector<MaskAndCost> &new_prev_subdomains);

    void get_next_meta_examples(MetaExample at_meta_example);
public:

    BitvectorTasks(int function_size, int task_id);

    BitvectorTasks(Task *_task_name, int _init_iter, int _num_iter, int _recursive_rep_set_depth, MetricType _metric,
                   ModeType _mode, int _min_mask_size, int _max_mask_size, int _num_prev_subtasks, string _dir_path,
                   bool _train_set_minimization, int _seed_train_set, int _num_minimization_steps,
                   double _init_minimization_fraction, double _end_minimization_fraction,
                   AutomatonRuleCost max_automaton_rule_cost);
};

#endif //COMPACTPOSET_BITVECTORTASKS_H

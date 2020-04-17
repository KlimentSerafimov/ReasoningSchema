//
// Created by Kliment Serafimov on 1/8/20.
//

#ifndef COMPACTPOSET_BITVECTORTASKS_H
#define COMPACTPOSET_BITVECTORTASKS_H

#include "ReasoningSchemaOptimizer.h"
#include "BittreeTypeExpression.h"

class MetaExample;

static const int max_instance_tree_degree = 8;

class InstanceTree
{
public:
    BittreeTaskType* instance;
    BittreeInputOutputType* delta;
    TaskName task_name;

    bool prepared_for_deepening = false;
    BittreeTaskType* superinstance_type;

    int num_superinstances;
    BittreeTaskType* superinstances[max_instance_tree_degree];

    bool deepened = false;
    vector<InstanceTree*> superinstance_trees;

    InstanceTree(BittreeTaskType* _instance, BittreeInputOutputType* _delta, TaskName _task_name);

    void prepare_for_deepening();

    void deepen();

    void populate_meta_examples(vector<vector<MetaExample> >& ret, int at_depth, int subtask_depth);
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

    int num_inputs;
    int function_size;

    vector<PartialFunction> ordering_over_boolean_functions;
    MetaExample get_meta_example(PartialFunction partial_function);

    void populate_ordering_over_boolean_functions_with_small_sum();
    void populate_ordering_over_boolean_functions_with_bitwise_and();

    vector<vector<Bitvector> > masks_generator(
            BittreeTypeExpression* type_expression, int num_iter, int num_subtasks, int max_mask_size, int min_mask_size);

    vector<vector<MetaExample> >
    get_meta_examples(BittreeTypeExpression *type_expression, TaskName task_name, int num_iter,
                      int subtask_depth);

public:

    BitvectorTasks(int function_size, int task_id);

    BitvectorTasks();
};

#endif //COMPACTPOSET_BITVECTORTASKS_H

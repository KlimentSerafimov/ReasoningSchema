//
// Created by Kliment Serafimov on 1/8/20.
//

#ifndef COMPACTPOSET_BITVECTORTASKS_H
#define COMPACTPOSET_BITVECTORTASKS_H

//#include "MetaExample.h"
#include "util.h"
#include <iostream>
#include "MinimalFactoringSchema.h"
#include "TaskName.h"
#include "BittreeTaskType.h"

class MetaExample;

static const int max_instance_tree_degree = 4;

class InstanceTree
{
public:
    BittreeTaskType* instance;
    DeltaBittreeTaskType* delta;
    TaskName task_name;

    bool prepared_for_deepening = false;
    BittreeTaskType superinstance_type;

    int num_superinstances;
    BittreeTaskType superinstances[max_instance_tree_degree];

    bool deepened = false;
    vector<InstanceTree*> superinstance_trees;

    InstanceTree(BittreeTaskType* _instance, DeltaBittreeTaskType* _delta, TaskName _task_name);

    void prepare_for_deepening();

    void deepen();

    void populate_meta_examples(vector<vector<MetaExample> > &ret, int at_depth);
};

class BittreeTaskInstance
{
public:

    BittreeTaskType* initialization;
    DeltaBittreeType* delta;

    BittreeTaskType curriculum[5];

    BittreeTaskType* bittree_task_type;

    vector<BittreeTaskType> instances;

    BittreeTaskInstance(BittreeTaskType* _bittree_task_type);
};

class BitvectorTasks {

    int num_inputs;
    int function_size;

    vector<PartialFunction> ordering_over_boolean_functions;
    void populate_ordering_over_boolean_functions_with_small_sum();
    void populate_ordering_over_boolean_functions_with_bitwise_and();
    MetaExample get_meta_example(PartialFunction partial_function);

public:

    BitvectorTasks(int function_size, int task_id);

    BitvectorTasks();
};

#endif //COMPACTPOSET_BITVECTORTASKS_H

//
// Created by Kliment Serafimov on 2019-11-20.
//

#ifndef COMPACTPOSET_DECISIONTREE_H
#define COMPACTPOSET_DECISIONTREE_H

#include "PartialFunction.h"
#include <map>

using namespace std;

static int contains_counter;

class DecisionTreeNode
{
public:
    int global_id = -1;
    NodeType node_type;

    //if node_type == internal_node
    DecisionTreeNode* branches[2] = {nullptr, nullptr};
    int idx;

    //if node_type == leaf_node
    int value;

    DecisionTreeNode();

    DecisionTreeNode(int _global_id);

    void my_delete();

    void apply_operation(SetOperationType operation_type, DecisionTreeNode *other, PartialFunction idx_to_branch);

    string to_string(int num_tabs, int num_inputs);

    void get_union_of_partial_functions(
            int num_inputs, vector<PartialFunction> &union_of_partial_functions, map<int, int> *idx_to_branch);

    void get_union_of_partial_functions_that_contain_partial_function(
            int num_inputs, PartialFunction partial_function, vector<PartialFunction> *union_of_partial_functions, map<int, int> *idx_to_branch);

    bool is_empty();

    bool contains(PartialFunction partial_function);
};

class DecisionTree {
    DecisionTreeNode* root = nullptr;

    void init(PartialFunction partial_function);

public:
    int num_inputs;
    DecisionTree();

    DecisionTree(int _num_inputs);

    DecisionTree(int _num_inputs, DecisionTreeNode *_root);

    DecisionTree(DecisionTree *to_point_to);

    DecisionTree(PartialFunction partial_function);

    DecisionTree(PartialFunction partial_function, SetOperationType operation_type, DecisionTree *other);

    DecisionTree copy();

    void my_delete();

    void apply_operation(SetOperationType operation_type, DecisionTree *other);

    DecisionTreeNode* get_root();

    bool is_empty();

    string to_string();

    string to_string(int num_tabs);

    vector<PartialFunction> get_union_of_partial_functions();

    void append_union_of_partial_functions_that_contain_partial_function(
            PartialFunction partial_function, vector<PartialFunction>* ret);

    string get_string_of_union_of_partial_functions(int num_tabs);

    bool contains(PartialFunction partial_function);
};

vector<pair<int, int> > get__idxs_and_branches(PartialFunction partial_function);

DecisionTreeNode* get_new_node();

int get__global_num_decision_tree_nodes();

int get__empty_slots_count();

#endif //COMPACTPOSET_DECISIONTREE_H

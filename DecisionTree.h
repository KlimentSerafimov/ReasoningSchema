//
// Created by Kliment Serafimov on 2019-11-20.
//

#ifndef COMPACTPOSET_DECISIONTREE_H
#define COMPACTPOSET_DECISIONTREE_H

#include "PartialFunction.h"
#include <map>

using namespace std;

static string operation_name[3] = {"difference", "intersection", "my_union"};

enum OperationType {difference, intersection, my_union};

enum NodeType {leaf_node, internal_node};

static int contains_counter;

class Node
{
public:
    int global_id = -1;
    NodeType node_type;

    //if node_type == internal_node
    Node* branches[2] = {NULL, NULL};
    int idx;

    //if node_type == leaf_node
    int value;

    Node();

    Node(int _global_id);

    void my_delete();

    void apply_operation(OperationType operation_type, Node *other, PartialFunction idx_to_branch);

    string to_string(int num_tabs, int num_inputs);

    void get_union_of_partial_functions(
            int num_inputs, vector<PartialFunction> &union_of_partial_functions, map<int, int> *idx_to_branch);

    void get_union_of_partial_functions_that_contain_partial_function(
            int num_inputs, PartialFunction partial_function, vector<PartialFunction> *union_of_partial_functions, map<int, int> *idx_to_branch);

    bool is_empty();

    bool contains(PartialFunction partial_function);
};

class DecisionTree {
    Node* root = NULL;

public:
    int num_inputs;
    DecisionTree();

    DecisionTree(int _num_inputs, Node *_root);

    DecisionTree(DecisionTree *to_point_to);

    DecisionTree(PartialFunction partial_function);

    DecisionTree(PartialFunction partial_function, OperationType operation_type, DecisionTree *other);

    void init(PartialFunction partial_function);

    DecisionTree copy();

    void my_delete();

    void apply_operation(OperationType operation_type, DecisionTree *other);

    Node* get_root();

    bool is_empty();

    string to_string(int num_inputs);

    string to_string(int num_tabs, int num_inputs);

    vector<PartialFunction> get_union_of_partial_functions(int num_inputs);

    void append_union_of_partial_functions_that_contain_partial_function(
            int num_inputs, PartialFunction partial_function, vector<PartialFunction>* ret);

    string get_string_of_union_of_partial_functions(int num_tabs);

    bool contains(PartialFunction partial_function);
};

vector<pair<int, int> > get__idxs_and_branches(PartialFunction partial_function);

Node* get_new_node();

int get__global_num_decision_tree_nodes();

int get__empty_slots_count();

#endif //COMPACTPOSET_DECISIONTREE_H

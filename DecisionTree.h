//
// Created by Kliment Serafimov on 2019-11-20.
//

#ifndef COMPACTPOSET_DECISIONTREE_H
#define COMPACTPOSET_DECISIONTREE_H

#include "PartialFunction.h"
#include <map>

using namespace std;

static string operation_name[2] = {"difference", "intersection"};

enum OperationType {difference, intersection};

enum NodeType {leaf_node, internal_node};

class Node
{
public:
    int global_id = -1;
    NodeType node_type;

    //if node_type == internal_node
    Node* branches[2];
    int idx;

    //if node_type == leaf_node
    int value;

    Node();

    Node(int _global_id);

    void my_delete();

    void apply_operation(OperationType operation_type, Node *other, map<int, int> *idx_to_branch);

    string to_string(int num_tabs, int num_inputs);

    void get_union_of_partial_functions(int num_inputs, vector<PartialFunction> &union_of_partial_functions,
                                        map<int, int> *idx_to_branch);
};

class DecisionTree {
    Node* root;

public:
    DecisionTree();

    DecisionTree(Node *_root);

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

    string get_string_of_union_of_partial_functions(int num_tabs, int num_inputs);
};

vector<pair<int, int> > get__idxs_and_branches(PartialFunction partial_function);

Node* get_new_node();

#endif //COMPACTPOSET_DECISIONTREE_H

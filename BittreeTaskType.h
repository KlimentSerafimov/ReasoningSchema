//
// Created by Kliment Serafimov on 1/11/20.
//

#ifndef COMPACTPOSET_BITTREETASKTYPE_H
#define COMPACTPOSET_BITTREETASKTYPE_H

#include "util.h"
#include "BitInBittree.h"
#include "DeltaBittreeType.h"
//#include "BitvectorTasks.h"
#include "TaskName.h"

class MetaExample;

class TreeNode
{
public:
    TreeNode* parent;
};

class BittreeTypeNode: public TreeNode
{
public:
    NodeType node_type;

    //if internal_node
    vector<BittreeTypeNode*>* children;

    BitInBittree *bit = NULL;
    //if bit_node;
    //no additional information needed;

    BittreeTypeNode(TreeNode* parent, NodeType _node_type);

    BittreeTypeNode(TreeNode* parent, NodeType _node_type, BitInBittreeType bit_type);

    BittreeTypeNode(TreeNode* parent, BittreeTypeNode* to_copy, bool all_new_bits);

    void init(TreeNode *_parent);

    string to_string();

    void apply_delta(DeltaBittreeType *delta_bittree_type);

//    void assign_bittree_type_node(BittreeTypeNode* new_value) {
//        node_type = new_value->node_type;
//        children->clear();
//        append_children_from(new_value);
//    }
    void append_children_from(BittreeTypeNode* new_value);

    void append_bits(vector<BitInBittree*>& bits);
};

class DeltaBittreeTaskType
{
public:
    DeltaBittreeType* delta_input = NULL;
    DeltaBittreeType* delta_output = NULL;
    DeltaBittreeTaskType(NodeType delta_input_type, NodeType delta_output_typee);
};

class BittreeTaskType: public TreeNode
{
public:
    BittreeTypeNode* input = NULL;
    BittreeTypeNode* output = NULL;

    BittreeTaskType* subtask = NULL;

    BittreeTaskType* solution = NULL;

    BittreeTaskType() = default;

    BittreeTaskType(NodeType input_node_type, NodeType output_node_type);

    BittreeTaskType(BittreeTaskType* to_copy, bool all_new_bits);

    void append_bits(vector<BitInBittree*>& bits);

    string to_string();

    void apply_delta(DeltaBittreeTaskType type) ;

    BittreeTaskType get_delta_application(DeltaBittreeTaskType type);

    BittreeTaskType get_supertask_type(DeltaBittreeTaskType type);

    void solve(TaskName task_name);

    void solve_sum();

    void solve_greater();

    void solve_cumulative_binary_operator();

    void bitwise_binary_operator();

    void do_multiplication_by(int multiply_by);

    MetaExample to_meta_example(int id);

    MetaExample to_meta_example_of_subtask_decomposition(int id);

};



#endif //COMPACTPOSET_BITTREETASKTYPE_H

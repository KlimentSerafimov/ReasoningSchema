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

enum BittreeTypeLeafNodeType {not_leaf_node, bit_node, double_node, delta_node};

class BittreeTypeNode: public TreeNode
{
public:
    NodeType node_type;

    //if internal_node
    vector<BittreeTypeNode*>* children;

    //if leaf_node;
    BittreeTypeLeafNodeType leaf_node_type;

    //if leaf_node_type == bit_node;
    BitInBittree *bit = NULL;

    //if leaf_node_type == double_node;

    //if leaf_nod_type == delta_node;
    BittreeTypeNode* delta;


    BittreeTypeNode(TreeNode* parent, NodeType _node_type);

    BittreeTypeNode(TreeNode* parent, NodeType _node_type, BitInBittreeType bit_type);

    BittreeTypeNode(TreeNode* parent, NodeType _node_type, BittreeTypeLeafNodeType leaf_node_type);

    BittreeTypeNode(TreeNode* parent, BittreeTypeNode* to_copy, bool all_new_bits);

    void copy_leaf_node(BittreeTypeNode *to_copy, bool all_new_bits);

    void init(TreeNode *_parent);

    string to_string();

    void apply_delta(BittreeTypeNode *delta_bittree_type);

//    void assign_bittree_type_node(BittreeTypeNode* new_value) {
//        node_type = new_value->node_type;
//        children->clear();
//        append_children_from(new_value);
//    }
    void append_children_from(BittreeTypeNode* new_value);

    void append_bits(vector<BitInBittree*>& bits);
};

//class DeltaBittreeTaskType
//{
//public:
//    DeltaBittreeType* delta_input = NULL;
//    DeltaBittreeType* delta_output = NULL;
//    DeltaBittreeTaskType(NodeType delta_input_type, NodeType delta_output_typee);
//};

enum BittreeTaskTypeType {base_type, delta_type};

class BittreeTaskType: public TreeNode
{
public:
    BittreeTaskTypeType bittree_task_type_type;

    BittreeTypeNode* input = NULL;
    BittreeTypeNode* output = NULL;

    BittreeTaskType* subtask = NULL;

    BittreeTaskType* solution = NULL;

    BittreeTaskType() = default;

    BittreeTaskType(NodeType input_node_type, NodeType output_node_type);

    BittreeTaskType(BittreeTaskType* to_copy, bool all_new_bits);

    void append_bits(vector<BitInBittree*>& bits);

    string to_string();

    void apply_delta(BittreeTaskType* type) ;

    BittreeTaskType get_delta_application(BittreeTaskType* type);

    BittreeTaskType get_supertask_type(BittreeTaskType* type);

    void solve(TaskName task_name);

    void solve__sum();

    void solve__greater();

    void solve__cumulative_binary_operator();

    void solve__bitwise_binary_operator();

    void solve__multiply_by(int multiply_by);

    void solve__one_shift_idx();

    void solve__count_unary();

    void solve__unary_sum();

    void solve__least_set_bit();

    void solve__max_window_between_bits();

    void solve__one_shift_idx__reverse_subtask(int init_size);

    void solve__count_unary__reverse_subtask(int init_size);

    MetaExample to_meta_example(int id);

    MetaExample to_meta_example_of_subtask_decomposition(int id);

};



class BittreeProgram
{

};



#endif //COMPACTPOSET_BITTREETASKTYPE_H

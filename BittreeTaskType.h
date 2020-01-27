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


class PartialFunction;
class MetaExample;

class BittreeTypeNode;
class BittreeTaskType;

enum BittreeTypeLeafNodeType {not_leaf_node, bit_node, double_node, delta_node};


class BittreeTypeNode: public TreeNode
{
public:
    NodeType node_type;

    //if internal_node
    vector<BittreeTypeNode*> children;

    //if leaf_node;
    BittreeTypeLeafNodeType leaf_node_type;

    //if leaf_node_type == bit_node;
    BitInBittree *bit = NULL;

    //if leaf_node_type == double_node;

    //if leaf_nod_type == delta_node;
    BittreeTypeNode* delta = NULL;

    BittreeTypeNode(TreeNode *parent, Name name, NodeType _node_type);

    BittreeTypeNode(TreeNode *parent, Name name, NodeType _node_type, BitInBittreeType bit_type);

    BittreeTypeNode(TreeNode *parent, Name name, NodeType _node_type, BittreeTypeLeafNodeType leaf_node_type);

    BittreeTypeNode(TreeNode *parent, Name name, BittreeTypeNode *to_copy, bool all_new_bits);

    BittreeTypeNode(TreeNode *parent, Name name, BittreeTypeNode *to_copy, bool all_new_bits,
                    bool hard_pointer_assign_bits);

    void copy_leaf_node(BittreeTypeNode *to_copy, bool all_new_bits);

    void copy_leaf_node(BittreeTypeNode *to_copy, bool all_new_bits, bool hard_pointer_assign_bits);

    void init();

//    string to_string();

    string to_string(int num_tabs);

    string bits_to_string(int num_tabs);

    void apply_delta(BittreeTypeNode *delta_bittree_type);

//    void assign_bittree_type_node(BittreeTypeNode* new_value) {
//        node_type = new_value->node_type;
//        children->clear();
//        append_children_from(new_value);
//    }

    void append_children_from(BittreeTypeNode* new_value);

    void append_bits(vector<BitInBittree*>& bits);

    BittreeTypeNode *set_delta(NodeType type, int i, BitInBittreeType bit_in_bittree_type);

    BittreeTypeNode *set_delta(NodeType delta_type_node, BittreeTypeLeafNodeType leaf_node_type);

    BittreeTypeNode * add_child(NodeType node_type, BitInBittreeType bit_in_bittree_type);
};

//class DeltaBittreeTaskType
//{
//public:
//    DeltaBittreeType* delta_input = NULL;
//    DeltaBittreeType* delta_output = NULL;
//    DeltaBittreeTaskType(NodeType delta_input_type, NodeType delta_output_typee);
//};

class BittreeTaskType: public TreeNode
{
public:

    BittreeTypeNode* input = NULL;
    BittreeTypeNode* output = NULL;

    BittreeTaskType* delta = NULL;
    BittreeTaskType* subtask = NULL;

    BittreeTaskType* solution = NULL;

    BittreeTaskType();

    BittreeTaskType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type);

    BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits);

    BittreeTaskType(TreeNode *_parent, BittreeTaskType *to_copy, bool all_new_bits, bool hard_pointer_assign_bits,
                    Name name);

    void append_bits(vector<BitInBittree*>& bits);

    void append_IO_bits(vector<BitInBittree*>& bits);

    string to_string();

    string to_string(int num_tabs);

    string bits_to_string(int num_tabs);

    void apply_delta(BittreeTaskType* type) ;

    BittreeTaskType get_delta_application(BittreeTaskType* type);

    BittreeTaskType* get_supertask_type(BittreeTaskType* type);

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

    PartialFunction to_partial_function();

    MetaExample to_meta_example(int id);

    MetaExample to_meta_example_of_subtask_decomposition(int id, int subtask_depth);

    BittreeTypeNode * add_input_child(NodeType child_type);

    BittreeTypeNode * add_output_child(NodeType child_type);

    BittreeTypeNode * add_output_child(NodeType child_type, BitInBittreeType bit_in_bittree_type);
};

class BittreeProgram
{
public:

    BittreeTaskType *root;
    int rec_depth;
    
    TreeNode* node = NULL;
    vector<Bitvector> bittree_masks_as_bitvectors;
    vector<BittreeProgram*> bittree_programs;
    vector<BittreeProgram*> next_rec_programs;

    bool is_root;

    //if is_root;
    vector<Bitvector> all_bittree_masks_as_bitvectors;

    BittreeProgram(TreeNode *_node, BittreeTaskType *root, int rec_depth);
    
    void populate_bittree_programs();
    
    void recurse_on_bittree_programs();
    
    void extract_partial_functions(vector<Bitvector>& ret);

};




#endif //COMPACTPOSET_BITTREETASKTYPE_H

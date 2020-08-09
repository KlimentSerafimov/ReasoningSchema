//
// Created by Kliment Serafimov on 1/11/20.
//

#ifndef COMPACTPOSET_BITTREETASKTYPE_H
#define COMPACTPOSET_BITTREETASKTYPE_H

#include "util.h"
#include "BitInBittree.h"
#include "DeltaBittreeType.h"
//#include "BitvectorTasks.h"
#include "Task.h"
#include "PartialFunction.h"


class PartialFunction;
class BittreeTaskTypeAsPartialFunction;
class MetaExample;
class BittreeNode;
class BittreeTaskType;

enum BittreeTypeLeafNodeType {not_leaf_node, bit_node, double_node, delta_node};


class BittreeNode: public TreeNode
{
public:
    NodeType node_type;

    //if internal_node
    vector<BittreeNode*> children;

    //if leaf_node;
    BittreeTypeLeafNodeType leaf_node_type;

    //if leaf_node_type == bit_node;
    BitInBittree *bit = NULL;

    //if leaf_node_type == double_node;

    //if leaf_nod_type == delta_node;
    BittreeNode* delta = NULL;

    BittreeNode(TreeNode *parent, Name name, NodeType _node_type);

    BittreeNode(TreeNode *parent, Name name, NodeType _node_type, BitInBittreeType bit_type);

    BittreeNode(TreeNode *parent, Name name, NodeType _node_type, BittreeTypeLeafNodeType leaf_node_type);

    BittreeNode(TreeNode *parent, Name name, BittreeNode *to_copy, bool all_new_bits);

    BittreeNode(TreeNode *parent, Name name, BittreeNode *to_copy, bool all_new_bits,
                bool hard_pointer_assign_bits);

    void copy_leaf_node(BittreeNode *to_copy, bool all_new_bits);

    void copy_leaf_node(BittreeNode *to_copy, bool all_new_bits, bool hard_pointer_assign_bits);

    void init();

//    string to_string();

    string to_string(int num_tabs);

    string bits_to_string(int num_tabs);

    void apply_delta(BittreeNode *delta_bittree_type);

//    void assign_bittree_type_node(BittreeNode* new_value) {
//        node_type = new_value->node_type;
//        children->clear();
//        append_children_from(new_value);
//    }

    void append_children_from(BittreeNode* new_value);

    void append_bits(vector<BitInBittree*>& bits);

    BittreeNode *set_delta(NodeType type, int i, BitInBittreeType bit_in_bittree_type);

    BittreeNode *set_delta(NodeType delta_type_node, BittreeTypeLeafNodeType leaf_node_type);

    BittreeNode *add_child(NodeType node_type, BitInBittreeType bit_in_bittree_type);

    void push_back_child(BittreeNode* child);

    string to_string__one_line();

    void populate_leaf_internals_and_bit_ids(vector<BittreeNode*> path, vector<pair<BittreeNode *, vector<int> > > & vector);
};

class BittreeInputOutputType: public TreeNode
{
public:
    BittreeNode* input = NULL;
    BittreeNode* output = NULL;

    BittreeInputOutputType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type);

    BittreeInputOutputType(TreeNode *_parent, Name name, BittreeInputOutputType *to_copy, bool all_new_bits);

    void append_bits(vector<BitInBittree*>& bits)
    {
        input->append_bits(bits);
        output->append_bits(bits);
    }

    string to_string(int num_tabs)
    {
        string ret = "";
        int next_tabs = num_tabs;
        int next_next_tabs = next_tabs+1;
        string open_bracket = "\n"+tabs(next_tabs) + "{\n";
        string close_bracket = tabs(next_tabs) + "}\n";
        ret += tabs(num_tabs)+"input"+input->TreeNode::to_string()+open_bracket + input->to_string(next_next_tabs) + close_bracket;
        ret += tabs(num_tabs)+"output"+output->TreeNode::to_string()+open_bracket + output->to_string(next_next_tabs) + close_bracket;
        return ret;
    }

    string bits_to_string(int num_tabs)
    {
        string ret = "";

        int next_tabs = num_tabs;
        int next_next_tabs = next_tabs+1;
        string open_bracket = "{\n";
        string close_bracket = tabs(num_tabs) + "}\n";
        ret += tabs(next_tabs)+"input"+open_bracket + input->bits_to_string(next_next_tabs) + close_bracket;
        ret += tabs(next_tabs)+"output"+open_bracket + output->bits_to_string(next_next_tabs) + close_bracket;

        return ret;
    }

    void apply_delta(BittreeInputOutputType* delta)
    {
        input->apply_delta(delta->input);
        output->apply_delta(delta->output);
    }

    BittreeNode * add_input_child(NodeType child_type);

    BittreeNode * add_output_child(NodeType child_type);

    BittreeNode * add_output_child(NodeType child_type, BitInBittreeType bit_in_bittree_type);

    void solve(Task *task_name);

};

class BittreeTaskDecomposition: public TreeNode
{
public:

    BittreeInputOutputType* delta = NULL;
    BittreeTaskType* subtask = NULL;

    BittreeTaskDecomposition(
            TreeNode *parent, Name name, BittreeInputOutputType *delta_to_copy, BittreeTaskType *subtask);

    BittreeTaskDecomposition(TreeNode *parent, Name name, BittreeTaskDecomposition* to_copy, bool all_new_bits);

    BittreeTaskDecomposition(TreeNode *parent, Name name, BittreeTaskDecomposition* to_copy, bool all_new_bits, bool copy_all);

    void append_bits(vector<BitInBittree*>& bits);

    void append_bits_of_prefix_subtree(vector<BitInBittree*>& bits, int num_subtasks);

    string to_string(int num_tabs);

    string bits_to_string(int num_tabs);
};

class BittreeTaskType: public TreeNode
{
public:

    BittreeInputOutputType* io = NULL;

    BittreeTaskDecomposition* decomposition = NULL;

    BittreeTaskType* solution = NULL;

    BittreeTaskType();

    BittreeTaskType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type);

    BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits);

    BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits, bool copy_all);

    void solve(Task *task_name);

    void append_bits(vector<BitInBittree*>& bits);

    void append_bits_of_prefix_subtree(vector<BitInBittree*>& bits, int num_subtasks);

    void append_IO_bits(vector<BitInBittree*>& bits);

    string to_string();

    string to_string(int num_tabs);

    string bits_to_string(int num_tabs);

    void apply_delta(BittreeInputOutputType* type) ;

    BittreeTaskType* get_supertask_type(BittreeInputOutputType* type);

    BittreeTaskTypeAsPartialFunction to_partial_function(int num_subtasks);

    MetaExample to_meta_example(int id, int num_subtasks);

    MetaExample to_meta_example_of_subtask_decomposition(int id, int subtask_depth);

    string to_string__one_line(int subtask_depth)
    {
        string ret = io->input->to_string__one_line();
        ret += io->output->to_string__one_line();
        int init_subtask_depth = subtask_depth;
        if(subtask_depth > 0) {
            BittreeTaskType* at_subtask = decomposition->subtask->solution;
            if(at_subtask == NULL)
            {
                at_subtask = decomposition->subtask;
            }
            while(subtask_depth>0)
            {
                ret += " ";
                ret += at_subtask->io->output->to_string__one_line();

                if(at_subtask->decomposition != NULL)
                {
                    assert(at_subtask->decomposition->subtask != NULL);
                    at_subtask = at_subtask->decomposition->subtask;
                }
                subtask_depth-=1;
            }
        }

        if(solution != NULL) {
            subtask_depth = init_subtask_depth;
            ret += "  -->  ";
            ret += solution->to_string__one_line(subtask_depth);
        }

//        ret += " -> ";
//        ret += solution->io->input->to_string__one_line();
//        ret += solution->io->output->to_string__one_line();

        return ret;
    }

    string to_string__one_line__first_part(int i);

    vector<MaskWithCost> generate_variety(int subtask_depth);
};

//class BittreeTaskTypeDecomposition: public TreeNode
//{
//    BittreeTaskInputOutputType* delta = NULL;
//    BittreeTaskInputOutputType* subtask = NULL;
//};
//
//class BittreeTaskType: public TreeNode
//{
//    BittreeTaskInputOutputType input_output;
//
//    BittreeTaskInputOutputType solution;
//
//    BittreeTaskTypeDecomposition* decomposition;
//};

class BittreeProgram
{
public:

    BittreeTaskType *root;
    int num_subtree_markers;
    int max_mask_type_depth;
    int num_subtasks;

    TreeNode* node = NULL;
    vector<Bitvector> bittree_masks_as_bitvectors;
    vector<BittreeProgram*> bittree_programs;
    vector<BittreeProgram*> next_rec_programs;

    bool is_root;

    //if is_root;
    vector<Bitvector> all_bittree_masks_as_bitvectors;

    BittreeProgram* get_child_bittree_program(TreeNode* child);

    BittreeProgram(TreeNode *_node, BittreeTaskType *root, int num_subtree_markers, int max_mask_type_depth, int num_subtasks);
    
    void populate_bittree_programs();
    
    void recurse_on_bittree_programs(int num_subtasks);
    
    void extract_partial_functions(vector<Bitvector>& ret);

};

#endif //COMPACTPOSET_BITTREETASKTYPE_H

// FOSSILS:

/*
    void solve__sum();

    void solve__greater();

    void solve__cumulative_binary_operator();

    void solve__bitwise_binary_operator();

    void solve__multiply_by(int multiply_by);

    void solve__add_by(int add_by);

    void solve__one_shift_idx();

    void solve__count_unary();

    void solve__unary_sum();

    void solve__least_set_bit();

    void solve__max_window_between_bits();

    void solve__max_window_between_bits_with_state();

    void solve__linear_and_or_expresson();

    void solve__linear_and_or_nand_nor__expression();

    void solve__sort_bits();

    void solve__one_shift_idx__reverse_subtask(int init_size);

    void solve__count_unary__reverse_subtask(int init_size);

    void solve__gene_network(int network);

    void solve__do_biggest_square(int width);*/


//
// Created by Kliment Serafimov on 1/11/20.
//

#ifndef COMPACTPOSET_BITTREETASKTYPE_H
#define COMPACTPOSET_BITTREETASKTYPE_H

#include "util.h"
#include "BitInBittree.h"
#include "BehaviorToProgram.h"

#include "Task.h"
#include "PartialFunction.h"
#include "BittreeNode.h"


class BittreeTaskDecomposition: public TreeNode
{
public:

    BittreeInputOutputType* delta = nullptr;
    BittreeTaskType* subtask = nullptr;

    BittreeTaskDecomposition(
            TreeNode *parent, Name name, BittreeInputOutputType *delta_to_copy, BittreeTaskType *subtask);

    BittreeTaskDecomposition(TreeNode *parent, Name name, BittreeTaskDecomposition* to_copy, bool all_new_bits);

    BittreeTaskDecomposition(TreeNode *parent, Name name, BittreeTaskDecomposition* to_copy, bool all_new_bits, bool copy_all);

    BittreeTaskDecomposition(TreeNode *parent, Name name, BittreeTaskDecomposition* to_copy, bool all_new_bits, bool copy_all, bool hard_all_new_bits);

    void append_bits(vector<BitInBittree*>& bits);

    void append_bits_of_prefix_subtree(vector<BitInBittree*>& bits, int num_subtasks);

    string to_string(int num_tabs);

    string bits_to_string(int num_tabs);
};

class BittreeTaskType: public TreeNode
{
public:

    BittreeInputOutputType* io = nullptr;

    BittreeTaskDecomposition* decomposition = nullptr;

    BittreeTaskType* solution = nullptr;

    BittreeTaskType();

    BittreeTaskType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type);

    BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits);

    BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits, bool copy_all);

    BittreeTaskType(TreeNode *_parent, Name name, BittreeTaskType *to_copy, bool all_new_bits, bool copy_all, bool hard_all_new_bits);

    void solve(Task *task_name);

    vector<MaskAndCostAndInstantiatedModules*> generate_variety(int subtask_depth, ofstream *fout, AutomatonRuleCost max_automaton_rule_cost, BehaviorToProgram* all_behavior);

    void append_bits(vector<BitInBittree*>& bits);

    void append_bits_of_prefix_subtree(vector<BitInBittree*>& bits, int num_subtasks);

    void append_IO_bits(vector<BitInBittree*>& bits);

    string to_string();

    string to_string(int num_tabs);

    string bits_to_string(int num_tabs);

    void apply_delta(BittreeInputOutputType* type) ;

    BittreeTaskType* get_supertask_type(BittreeInputOutputType* delta);

    BittreeTaskTypeAsPartialFunction to_partial_function(int num_subtasks);

    MetaExample to_meta_example(int id, int num_subtasks);

    MetaExample to_meta_example_of_subtask_decomposition(int id, int subtask_depth);

    string to_string__one_line(int subtask_depth);

    string to_string__one_line__first_part(int i);

    int get_function_size();
};

//class BittreeTaskTypeDecomposition: public TreeNode
//{
//    BittreeTaskInputOutputType* delta = nullptr;
//    BittreeTaskInputOutputType* subtask = nullptr;
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

    TreeNode* node = nullptr;
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

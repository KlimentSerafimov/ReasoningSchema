//
// Created by Kliment Serafimov on 8/10/20.
//

#ifndef COMPACTPOSET_BITTREENODE_H
#define COMPACTPOSET_BITTREENODE_H

#include "util.h"
#include "BitInBittree.h"
#include "AutomatonRuleCost.h"
#include "PrimitiveRule.h"
#include "BehaviorToProgram.h"

class PartialFunction;
class BittreeTaskTypeAsPartialFunction;
class MetaExample;
class BittreeNode;
class BittreeTaskType;
class CanvasAndBittreeProgram;
class SequenceOfPrimitiveRules;
class PrimitiveRuleContext;

enum BitTypeOverride {};

enum BittreeLeafNodeType {not_leaf_node, bit_node, double_node, delta_node};

class BittreeNode: public TreeNode
{

public:
    NodeType node_type;

    //if internal_node
    vector<BittreeNode*> children;

    //if leaf_node;
    BittreeLeafNodeType leaf_node_type;

    //if leaf_node_type == bit_node;
    BitInBittree *bit = nullptr;

    bool is_bit() const
    {
        return node_type == leaf_node && leaf_node_type == bit_node;
    }

    //if leaf_node_type == double_node;

    //if leaf_nod_type == delta_node;
    BittreeNode* delta = nullptr;

    int subtree_sum = -1;

    BittreeNode(TreeNode *parent, Name name, NodeType _node_type);

    BittreeNode(TreeNode *parent, Name name, NodeType _node_type, BitInBittreeType bit_type);

    BittreeNode(TreeNode *parent, Name name, NodeType _node_type, BittreeLeafNodeType leaf_node_type);

    BittreeNode(TreeNode *parent, Name name, BittreeNode *to_copy, bool all_new_bits);

    BittreeNode(TreeNode *parent, Name name, BittreeNode *to_copy, bool all_new_bits, bool hard_pointer_assign_bits);

    BittreeNode(TreeNode *parent, Name name, BittreeNode *to_copy, bool all_new_bits, bool hard_pointer_assign_bits, bool hard_all_new_bits);

    void copy_leaf_node(BittreeNode *to_copy, bool all_new_bits);

    void copy_leaf_node(BittreeNode *to_copy, bool all_new_bits, bool hard_pointer_assign_bits,
                        bool hard_all_new_bits);

    void init();

    string to_string(int num_tabs);

    string bits_to_string(int num_tabs);

    void apply_delta(BittreeNode *delta_bittree_type);

    void append_children_from(BittreeNode* new_value);

    void append_bits(vector<BitInBittree*>& bits);

    BittreeNode *set_delta(NodeType type, int i, BitInBittreeType bit_in_bittree_type);

    BittreeNode *set_delta(NodeType delta_type_node, BittreeLeafNodeType leaf_node_type);

    BittreeNode *set_delta(BittreeNode *_delta);

    BittreeNode *add_child(NodeType node_type, BitInBittreeType bit_in_bittree_type);

    BittreeNode * push_back_child(BittreeNode* child);

    string to_string__one_line() const;

    void populate_leaf_internals_and_bit_ids(vector<BittreeNode*> path, vector<pair<BittreeNode *, vector<int> > > & vector);

    string slim_tree_to_string(int tab) const;

    void apply_all_rules_to_subtree(
            vector<SequenceOfPrimitiveRules> *rules, CanvasAndBittreeProgram *canvas, vector<int> *path,
            AutomatonRuleCost max_cost, BehaviorToProgram *all_programs) const;

    void populate_programs(
            vector<SequenceOfPrimitiveRules> *rules,
            CanvasAndBittreeProgram *canvas,
            int next_child,
            vector<int> *path,
            BehaviorToProgram *all_programs,
            AutomatonRuleCost max_cost) const;

    pair<BittreeNode *, BittreeNode*>
    apply_rule(SequenceOfPrimitiveRules sequence_of_rules, BittreeNode *canvas, BittreeNode *pointer_on_canvas, vector<int> *path);

    pair<BittreeNode *, vector<int> *>
    apply_primitive_rule_to_bit(PrimitiveRule rule, BittreeNode *pointer_on_canvas, vector<int>* path) const;

    void initialize_special_parents(BittreeNode *parent);

    vector<int> get_root_to_node_path(BittreeNode* canvas);

    BittreeNode * get_node(const vector<int> * path, int depth);

    BittreeNode * get_node(const vector<int> * path);

    int calc_subtree_sums();

    Bitvector to_bitvector()
    {
        assert(node_type == internal_node);
        for(int i = 0;i<children.size();i++)
        {
            assert(children[i]->is_bit());
        }
        Bitvector ret = Bitvector(0, children.size());
        for(int i = 0;i<children.size();i++)
        {
            assert(children[i]->is_bit());
            assert(children[i]->bit->is_bit_set);

            ret.set(i, children[i]->bit->bit_val);
        }
        return ret;
    }
};

void add_child(NodeType type, BitInBittreeType type1);


#endif //COMPACTPOSET_BITTREENODE_H

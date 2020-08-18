//
// Created by Kliment Serafimov on 8/18/20.
//

#include <set>
#include "AutomatonRuleCost.h"
#include "MetaExample.h"
#include "BittreeTaskType.h"
#include "BittreeInputOutputType.h"

BittreeInputOutputType::BittreeInputOutputType(TreeNode *_parent, Name name, NodeType input_node_type, NodeType output_node_type)
        : TreeNode(_parent, name, this)
{
    input = new BittreeNode(this, Name("input"), input_node_type);
    output = new BittreeNode(this, Name("output"), output_node_type);
}

BittreeInputOutputType::BittreeInputOutputType(TreeNode *_parent, Name name, BittreeInputOutputType *to_copy, bool all_new_bits)
        : TreeNode(_parent, name, this)
{
    copied_from = to_copy;
    to_copy->copies.push_back(this);
    input = new BittreeNode(this, Name("input"), to_copy->input, all_new_bits);
    output = new BittreeNode(this, Name("output"), to_copy->output, all_new_bits);
}

void BittreeInputOutputType::solve(Task *task_name)
{
    task_name->solve(this);
}

BittreeNode* BittreeInputOutputType::add_input_child(NodeType child_type) {
    assert(input != nullptr);
    BittreeNode* new_child = new BittreeNode(input, Name("children", input->children.size()), child_type);
    input->children.push_back(new_child);
    return new_child;
}

BittreeNode* BittreeInputOutputType::add_output_child(NodeType child_type) {
    assert(output != nullptr);
    BittreeNode* new_child =
            new BittreeNode(
                    output, Name("children", input->children.size()), child_type);
    output->children.push_back(new_child);
    return new_child;
}

BittreeNode* BittreeInputOutputType::add_output_child(NodeType child_type, BitInBittreeType bit_in_bittree_type) {
    assert(output != nullptr);
    BittreeNode* new_child =
            new BittreeNode(
                    output, Name("children", output->children.size()), child_type, bit_in_bittree_type);
    output->children.push_back(new_child);
    return new_child;
}
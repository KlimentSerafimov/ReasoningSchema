//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "Node.h"
#include "MetaExample.h"
#include <vector>
#include <map>
#include <iostream>
#include <queue>
#include "util.h"

using namespace std;

CompactPosetNode::CompactPosetNode(PartialFunction partial_function) : DecisionTree(partial_function) {
}

CompactPosetNode::CompactPosetNode(PartialFunction partial_function, OperationType operation_type, DecisionTree *other)
        : DecisionTree(partial_function, operation_type, other) {
}

CompactPosetNode::CompactPosetNode(DecisionTree partial_function): DecisionTree(partial_function.get_root()){

}


CompactPosetNode::CompactPosetNode(DecisionTree *partial_function): DecisionTree(partial_function->get_root()){

}

CompactPosetNode::CompactPosetNode(CompactPosetNode* compact_poset_node) : DecisionTree((*compact_poset_node)){
    id_in_compact_poset = compact_poset_node->id_in_compact_poset;
}

CompactPosetNode CompactPosetNode::copy() {
    CompactPosetNode ret = CompactPosetNode(DecisionTree::copy());
    ret.id_in_compact_poset = id_in_compact_poset;
    return ret;
}

void CompactPosetNode::apply_operation(OperationType operation_type, CompactPosetNode *other) {
    DecisionTree::apply_operation(operation_type, other);
    applied_operations.push_back(AppliedOperation(operation_type, id_in_compact_poset, other->id_in_compact_poset));
}

void CompactPosetNode::my_delete() {
    node_type = inactive_node;
    applied_operations.clear();
    DecisionTree::my_delete();
    downstream_union.my_delete();
    dominator_union.my_delete();
}

static string node_type_name[2] = {"base_node", "union_node"};

string CompactPosetNode::to_string(int i, int num_inputs)
{
    string ret;
    ret += tabs(1) + "id = " + std::to_string(i) + "\n";
    ret += tabs(1) + "node_type = " + node_type_name[node_type] + "\n";
    ret += tabs(1) + "union_of_partial_functions = \n";
    ret += get_string_of_union_of_partial_functions(2, num_inputs);
    ret += tabs(1) + "applied_operations = \n";
    for(int j = 0;j<applied_operations.size();j++)
    {
        ret += tabs(2) + applied_operations[j].to_string() + "\n";
    }
    ret += "\n";
//        ret += tabs(1) + "decision_tree: \n";
//        ret += nodes[i].to_string(2, num_inputs) + "\n";

    return ret;
}

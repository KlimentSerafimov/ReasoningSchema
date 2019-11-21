//
// Created by Kliment Serafimov on 2019-11-18.
//

#include "Node.h"
#include "MetaExample.h"
#include <vector>
#include <map>
#include <iostream>
#include <queue>

using namespace std;

CompactPosetNode::CompactPosetNode(PartialFunction partial_function) : DecisionTree(partial_function) {
}

CompactPosetNode::CompactPosetNode(PartialFunction partial_function, OperationType operation_type, DecisionTree *other)
        : DecisionTree(partial_function, operation_type, other) {
}

CompactPosetNode::CompactPosetNode(DecisionTree partial_function): DecisionTree(partial_function.get_root()){

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

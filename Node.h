//
// Created by Kliment Serafimov on 2019-11-18.
//

#ifndef COMPACTPOSET_NODE_H
#define COMPACTPOSET_NODE_H


#include "MetaExample.h"
#include <vector>
#include "util.h"
#include "DecisionTree.h"

using namespace std;

class AppliedOperation
{
    OperationType operation_type;
    int left_id;
    int right_id;

public:
    AppliedOperation(OperationType _operation_type, int _left_id, int _right_id)
    {
        operation_type = _operation_type;
        left_id = _left_id;
        right_id = _right_id;
    }
    string to_string()
    {
        return std::to_string(left_id) + " " + operation_name[operation_type] + " " + std::to_string(right_id);
    }
};

enum CompactPosetNodeType {base_node, union_node, inactive_node};
static string node_type_name[2] = {"base_node", "union_node"};

class CompactPosetNode: public DecisionTree
{
public:

    CompactPosetNodeType node_type = base_node;

    int id_in_compact_poset = -1;

    vector<AppliedOperation> applied_operations;

    CompactPosetNode() = default;

    explicit CompactPosetNode(PartialFunction partial_function);

    explicit CompactPosetNode(DecisionTree partial_function);

    explicit CompactPosetNode(CompactPosetNode *compact_poset_node);

    CompactPosetNode(PartialFunction partial_function, OperationType operation_type, DecisionTree *other);

    CompactPosetNode copy();

    void apply_operation(OperationType operation_type, CompactPosetNode *other);

};

#endif //COMPACTPOSET_NODE_H

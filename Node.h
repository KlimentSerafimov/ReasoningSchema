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
    SetOperationType operation_type;
    int left_id;
    int right_id;

public:
    AppliedOperation(SetOperationType _operation_type, int _left_id, int _right_id)
    {
        operation_type = _operation_type;
        left_id = _left_id;
        right_id = _right_id;
    }
    string to_string()
    {
        return std::to_string(left_id) + " " + set_operation_name[operation_type] + " " + std::to_string(right_id);
    }
};


static string node_type_name[3] = {"base_node", "union_node", "inactive_node"};

enum CompactPosetNodeType {base_node, union_node, inactive_node};

template<typename BooleanFunctionSet>
class CompactPosetNode: public BooleanFunctionSet
{
public:

    CompactPosetNodeType node_type = base_node;

    int id_in_compact_poset = -1;

    vector<AppliedOperation> applied_operations;

    BooleanFunctionSet downstream_union;
    BooleanFunctionSet dominator_union;

    int open_visited_mark = -1;
    int closed_visited_mark = -1;
    int dominated_mark = -1;

    int num_incoming_meta_edges = 0;
    int num_incoming_union_edges = 0;

    CompactPosetNode() = default;

    explicit CompactPosetNode(PartialFunction partial_function) : BooleanFunctionSet(partial_function) {}

    explicit CompactPosetNode(BooleanFunctionSet partial_function) : BooleanFunctionSet(&partial_function) {}

    explicit CompactPosetNode(BooleanFunctionSet *partial_function) : BooleanFunctionSet(partial_function) {}

    explicit CompactPosetNode(CompactPosetNode<BooleanFunctionSet> *compact_poset_node) :
        BooleanFunctionSet((*compact_poset_node))
    {
        id_in_compact_poset = compact_poset_node->id_in_compact_poset;
    }

    explicit CompactPosetNode(PartialFunction partial_function, SetOperationType operation_type, BooleanFunctionSet *other) :
            BooleanFunctionSet(partial_function, operation_type, other) {}

    CompactPosetNode<BooleanFunctionSet> copy()
    {
        CompactPosetNode<BooleanFunctionSet> ret = CompactPosetNode<BooleanFunctionSet>(BooleanFunctionSet::copy());
        ret.id_in_compact_poset = id_in_compact_poset;
        return ret;
    }

    void apply_operation(SetOperationType operation_type, CompactPosetNode<BooleanFunctionSet> *other)
    {
        BooleanFunctionSet::apply_operation(operation_type, other);
        applied_operations.push_back(AppliedOperation(operation_type, id_in_compact_poset, other->id_in_compact_poset));
    }

    void my_delete()
    {
        node_type = inactive_node;
        applied_operations.clear();
        BooleanFunctionSet::my_delete();
        downstream_union.my_delete();
        dominator_union.my_delete();
    }

    string to_string(int i, int num_inputs)
    {
        string ret;
        ret += tabs(1) + "id = " + std::to_string(i) + "\n";
        ret += tabs(1) + "node_type = " + node_type_name[node_type] + "\n";
        ret += tabs(1) + "union_of_partial_functions = \n";
        ret += BooleanFunctionSet::get_string_of_union_of_partial_functions(2);
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


};

#endif //COMPACTPOSET_NODE_H

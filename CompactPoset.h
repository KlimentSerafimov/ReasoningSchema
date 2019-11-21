//
// Created by Kliment Serafimov on 2019-11-18.
//


#ifndef COMPACTPOSET_COMPACTPOSET_H
#define COMPACTPOSET_COMPACTPOSET_H

#include <vector>
#include "Node.h"

using namespace std;

class DeltaCompactPoset
{
};

class CompactPoset {

    int num_inputs;

    vector<CompactPosetNode> nodes;
    vector<vector<int> > is_union_of;
    vector<vector<int> > is_contained_in;
    vector<vector<int> > meta_edges;

    int visited_mark = 0;
    vector<int> open_visited;
    vector<int> closed_visited;

    vector<int> dominator_stack;

public:
    CompactPoset(int num_inputs);

    int push_back_new_node__and__get_id(CompactPosetNode decision_tree);

    int push_back_new_node__and__get_id(CompactPosetNode decision_tree, vector<int> to_union_nodes);

    pair<int, int> split_node(int node_id, CompactPosetNode* first, CompactPosetNode* second);

    bool append(MetaExample meta_example);

    void pop();

    void split_nodes__and__insert_meta_edge_in_poset(CompactPosetNode *dominated, int id__dominator_base,
                                                         vector<int> ids__dominated_base);

    bool is_valid_node(int at);

    bool visit(int at);

    bool is_valid();

    void create_union_edge(int container, int contained);

    string to_string();

    void print();

    void print_operation(OperationType operation_type, CompactPosetNode* first, CompactPosetNode* second);
};


#endif //COMPACTPOSET_COMPACTPOSET_H

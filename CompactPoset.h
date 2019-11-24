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
public:
    bool was_poped = false;
    MetaExample meta_example;
    vector<int> new_nodes;
    vector<int> new_union_nodes;
    pair<int, int> new_meta_edge;
    vector<pair<int, int> > new_union_edges;
    DeltaCompactPoset() = default;
};

class CompactPoset {

    int num_inputs;

    vector<CompactPosetNode> nodes;
    vector<vector<int> > is_union_of;
    vector<vector<int> > is_contained_in;
    vector<vector<int> > meta_edges;
    vector<vector<int> > reverse_meta_edges;

    int visited_mark = 0;
    vector<int> open_visited;
    vector<int> closed_visited;

    vector<DeltaCompactPoset> delta_stack;

    DeltaCompactPoset local_delta;

    vector<int> min_cycles;

    int num_meta_edges = 0;

    vector<pair<int, pair<vector<int>::iterator, vector<int>::iterator> > > removed_edges;

public:
    CompactPoset(int num_inputs);

    CompactPoset(CompactPoset *poset);

    int push_back_new_node__and__get_id(CompactPosetNode decision_tree);

    int push_back_new_node__and__get_id(CompactPosetNode decision_tree, vector<int> to_union_nodes);

    bool append(MetaExample meta_example);

    void pop();

    void simple_pop();

    int split_dominated_base(CompactPosetNode *dominated, int id__dominator_base,
                             vector<int> ids__dominated_base);

    bool is_valid_node(int at);

    bool visit(int at);

    bool is_valid();

    bool there_exist_redundant_meta_edge();

    int get_loop(int at, int count);

    int get_loop_from_node(int origin);

    void mark_dominated_init(int origin);

    void mark_dominated(int origin);

    bool there_are_new_dominated(int at);

    bool there_are_new_dominated__init(int origin);

    void create_union_edge(int container, int contained);

    void make_union_node(int new_union_node);

    string to_string();

    void print();

    void print_operation(OperationType operation_type, CompactPosetNode* first, CompactPosetNode* second);

    int get_num_nodes();

    int get_num_meta_edges();

    int get_num_inserts();

    void disjoint_assertion();

    int get_num_inputs();

    vector<MetaExample> get_meta_examples();

    bool empty();

    void clear();

    void add_edges_back();
};


#endif //COMPACTPOSET_COMPACTPOSET_H

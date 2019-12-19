//
// Created by Kliment Serafimov on 2019-11-18.
//


#ifndef COMPACTPOSET_COMPACTPOSET_H
#define COMPACTPOSET_COMPACTPOSET_H

#include <vector>
#include "Node.h"
#include <set>

using namespace std;

class DeltaCompactPoset
{
public:
    bool was_popped = false;
    MetaExample meta_example;
    vector<int> new_nodes;
    vector<int> new_union_nodes;
    pair<int, int> new_meta_edge;
    pair<vector<int>::iterator, vector<int>::iterator> soft_delete_iterators;
    vector<pair<int, int> > new_union_edges;
    bool removed_after_the_fact = false;
    bool not_unique = false;
    DeltaCompactPoset() = default;
};

class CompactPoset {

    int num_inputs;
    int generalization_mask;
    int input_mask;

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

    int cycle_init_id;
    vector<int> min_cycles;

    map<pair<int, int>, int> edge_to_meta_example_id;

    int num_meta_edges = 0;

    vector<pair<int, pair<vector<int>::iterator, vector<int>::iterator> > > removed_edges;

    set<pair<int, pair<int, int> > > uniques;


public:

    vector<int> meta_example_ids_in_cycle;

    int num_visited_nodes_during_query = 0;

    CompactPoset() = default;

    CompactPoset(int num_inputs);

    CompactPoset(int num_inputs, int generalization_mask, int input_mask);

    CompactPoset(CompactPoset *poset);

    int push_back_new_node__and__get_id(CompactPosetNode decision_tree);

    int push_back_new_node__and__get_id(CompactPosetNode decision_tree, vector<int> to_union_nodes);

    vector<int> get__set_cover(CompactPosetNode *target_set);

    int multi_split_and_union(vector<int> ids__base_node_cover, CompactPosetNode *target_set);

    bool insert(MetaExample meta_example);

    void pop();

    vector<MetaExample> get_necessary_meta_examples();

    vector<MetaExample> get_all_meta_examples_without_duplicates();

    void simple_pop();

    int compress();

    int soft_delete_meta_example_with_idx(int meta_example_idx);

    void soft_delete(int id);

    void re_insert(int id);

    DecisionTree* get_downstream_union(int at);

    void calc_downstream_unions();

    DecisionTree* get_dominator_union(int at);

    void calc_dominator_unions();

    vector<PartialFunction> query(PartialFunction partial_function);

    bool is_valid_node(int at);

    bool visit(int at);

    bool is_valid();

    bool there_exist_redundant_meta_edge();

    bool there_exist_redundant_meta_edge__partial(PartialFunction partial_function);

    int get_loop(int at, int count);

    int get_loop_from_node(int origin);

    void mark_dominated_init(int origin);

    void mark_dominated(int origin, bool is_origin);

    bool there_are_new_dominated(int at, bool is_origin);

    bool there_are_new_dominated__init(int origin);

    void create_union_edge(int container, int contained);

    void make_union_node(int new_union_node);

    string meta_examples_to_string();

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

    void classify_nodes();
};


#endif //COMPACTPOSET_COMPACTPOSET_H

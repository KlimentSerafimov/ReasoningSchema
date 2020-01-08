//
// Created by Kliment Serafimov on 2019-11-18.
//


#ifndef COMPACTPOSET_COMPACTPOSET_H
#define COMPACTPOSET_COMPACTPOSET_H

#include <vector>
#include "Node.h"
#include <set>
#include "UnionOfPartialFunctions.h"

using namespace std;

//typedef DecisionTree BooleanFunctionSet;

typedef UnionOfPartialFunctions BooleanFunctionSet;

typedef CompactPosetNode<BooleanFunctionSet> NewCompactPosetNode;

class DeltaCompactPoset
{
public:
    MetaExample meta_example;
    vector<int> equivalent_meta_edge_ids;

    vector<int> new_nodes;
    vector<int> new_union_nodes;
    pair<int, int> new_meta_edge;
    pair<vector<int>::iterator, vector<int>::iterator> soft_delete_iterators;
    vector<pair<int, int> > new_union_edges;

    bool removed_after_the_fact = false;
    bool not_unique = false;
    bool soft_deleted = false;

    bool removed_from_uniques = false;
    DeltaCompactPoset() = default;

    bool is_existant()
    {
        return !(
                removed_after_the_fact ||
                soft_deleted ||
                not_unique);
    }
};

class CompactPoset {

    int function_size;
    int generalization_mask;
    int input_mask;

    vector<NewCompactPosetNode> nodes;
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

    map<pair<int, int>, int> edge_to_delta_stack_id;

    vector<int> meta_edges_ids_in_cycle;

    int num_meta_edges = 0;

    vector<int> removed_edges;

    map<pair<int, pair<int, int> >, int> uniques;

    void push_back_meta_edge(int dominator, int dominated, int meta_edge_id);

    void re_insert_meta_edge(int meta_edge_id);

    void pop_meta_edge(int dominator_node, int dominated_node);

    pair<vector<int>::iterator, vector<int>::iterator> erase_meta_edge(int dominator, int dominated);

    int push_back_new_node__and__get_id(NewCompactPosetNode new_compact_poset_node);

    vector<int> get__set_cover(NewCompactPosetNode *target_set);

    int multi_split_and_union(vector<int> ids__base_node_cover, NewCompactPosetNode *target_set);

    BooleanFunctionSet* get_downstream_union(int at);

    BooleanFunctionSet* get_dominator_union(int at);

    bool visit(int at);

    int get_loop(int at, int count);

    int get_loop_from_node(int origin);

    void mark_dominated_init(int origin);

    void mark_dominated(int origin, bool is_origin);

    bool there_are_new_dominated(int at, bool is_origin);

    bool there_are_new_dominated__init(int origin);

    void create_union_edge(int container, int contained);

    void make_union_node(int new_union_node);

    void print_operation(SetOperationType operation_type, NewCompactPosetNode* first, NewCompactPosetNode* second);

    void erase_from_uniques(int id);

public:


    int num_visited_nodes_during_query = 0;

    CompactPoset() = default;

    CompactPoset(int _function_size);

    CompactPoset(int _function_size, vector<MetaExample> &meta_examples);

    CompactPoset(int _function_size, int generalization_mask, int input_mask);

    CompactPoset(int _function_size, int generalization_mask, int input_mask, vector<MetaExample> &meta_examples);

    CompactPoset(CompactPoset *poset);

    vector<MetaExample> insert_meta_examples_and_get_inserted(vector<MetaExample> &meta_examples);

    bool insert(MetaExample meta_example);

    void hard_pop();

    vector<MetaExample> get_existant_meta_examples();

    vector<MetaExample> get_all_meta_examples_without_duplicates();

    void soft_pop();

    int soft_delete_meta_example_with_idx(int meta_example_idx);

    void soft_delete(int id);

    void re_insert(int id);

    void calc_downstream_unions();

    void calc_dominator_unions();

    vector<PartialFunction> query(PartialFunction partial_function);

    bool is_valid_node(int at);

    bool is_valid();

    bool soft_delete_redundant_edges();

    pair<vector<int>, vector<int>> get_cycle_of_meta_example_ids_and_meta_edge_ids();

    string meta_examples_to_string();

    string to_string();

    void print();

    int get_num_nodes();

    int get_num_meta_edges();

    int get_num_inserts();

    void disjoint_assertion();

    int get_function_size();

    vector<MetaExample> get_meta_examples();

    vector<int> get_existant_meta_example_ids();

    void get_existant_meta_examples_and_equivalent_ids(
            vector<MetaExample> &ret_meta_examples, vector<vector<int> > &ret_equivalent_ids);

    bool empty();

    void clear();

    void add_edges_back();

    void compare_nodes();
};


#endif //COMPACTPOSET_COMPACTPOSET_H

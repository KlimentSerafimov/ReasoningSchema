//
// Created by Kliment Serafimov on 2019-12-18.
//

#ifndef COMPACTPOSET_MINIMALFACTORINGSCHEMA_H
#define COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

#include <vector>
#include "CompactPoset.h"
#include "MetaExample.h"


class MinimalFactoringSchema
{
    MinimalFactoringSchema* parent_pointer;
    int num_inputs;
    int function_size;

    vector<MetaExample> meta_examples;
    int init_num_missing_bits;

    vector<vector<vector<MetaExample> > > globally_consistent_meta_examples;
    vector<vector<vector<vector<int> > > > equivalent_meta_example_ids;

    int best_mask_size;
    int best_mask_id;
    int best_necessary_num_meta_examples;

    vector<MetaExample> get_meta_example_ids_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
            int subdomain_mask);

    vector<MetaExample> get_meta_examples_after_query(int subdomain_mask, CompactPoset *compact_poset, bool print);

    bool test_compact_poset_for_consistency_with_all_meta_examples(int subdomain_mask, CompactPoset *compact_poset);

    pair<vector<MetaExample>, vector<vector<int> > > get_globally_consistent_meta_examples_and_equivalent_ids(int subdomain_mask,
                                                                                 CompactPoset *compact_poset);

    int get_meta_edge_id_to_remove(CompactPoset* compact_poset, int subdomain_mask, int special_meta_example_id);

    pair<double, pair<int, vector<int> > > calculate_heuristic(int current_number_missing_bits, int mask_size, int mask_id);

public:
    MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples,
                               MinimalFactoringSchema *parent_pointer);
};



#endif //COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

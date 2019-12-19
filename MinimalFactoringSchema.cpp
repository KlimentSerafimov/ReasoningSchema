//
// Created by Kliment Serafimov on 2019-12-18.
//

#include "MinimalFactoringSchema.h"
#include <iostream>

vector<int> MinimalFactoringSchema::
        get_meta_example_ids_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
        int subdomain_mask, CompactPoset *local_compact_poset) {

    vector<int> consistent_meta_examples;

    for (int insert_meta_example_id = 0; insert_meta_example_id < meta_examples.size(); insert_meta_example_id++) {
        assert(local_compact_poset->get_num_meta_edges() == 0);
        MetaExample local_insert_meta_example =
                meta_examples[insert_meta_example_id].get_application_of_subdomain(subdomain_mask);
        if (!local_insert_meta_example.fully_constrained()) {
            local_compact_poset->insert(local_insert_meta_example);
            bool is_consistent = true;
            for (int test_meta_example_id = 0;
                 test_meta_example_id < meta_examples.size(); test_meta_example_id++) {
                MetaExample local_test_meta_example =
                        meta_examples[test_meta_example_id].get_application_of_subdomain(subdomain_mask);
                if (!local_test_meta_example.fully_constrained()) {
                    if (!local_compact_poset->insert(local_test_meta_example)) {
                        local_compact_poset->pop();
                        is_consistent = false;
                        break;
                    }
                    local_compact_poset->pop();
                }
            }
            if (is_consistent) {
                consistent_meta_examples.push_back(insert_meta_example_id);
            }
            local_compact_poset->pop();
        }
    }
    return consistent_meta_examples;
}


MinimalFactoringSchema::MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples) {

    time_t local_time = (double) time(nullptr);

    num_inputs = _num_inputs;
    function_size = (1 << num_inputs);
    meta_examples = _meta_examples;
    vector<vector<int> > masks_by_size = vector<vector<int> >(function_size + 1, vector<int>());

    vector<vector<vector<CompactPoset> > > compact_posets_per_masks_size_per_mask_id_per_reasoning_step =
            vector<vector<vector<CompactPoset> > >(function_size + 1, vector<vector<CompactPoset> >());

    vector<int> unit_masks;
    vector<CompactPoset> compact_posets;

    for (int i = 1; i < (1 << function_size); i++) {
        masks_by_size[__builtin_popcount(i)].push_back(i);
        compact_posets_per_masks_size_per_mask_id_per_reasoning_step[__builtin_popcount(i)].push_back(
                vector<CompactPoset>());
    }

    for (int mask_size = 4; mask_size < masks_by_size.size(); mask_size++) {
        cout << "working on masks_size = " << mask_size << endl;
        vector<pair<int, int> > masks_ids_by_contained_meta_examples;
        vector<vector<int> > consistent_meta_examples_per_mask_id;
        for (int mask_id = 0; mask_id < masks_by_size[mask_size].size(); mask_id++) {
            cout << "working on mask = " << bitvector_to_str(masks_by_size[mask_size][mask_id], function_size) << " time: " << (double) time(nullptr) - local_time << endl;

            int local_compact_poset_id = compact_posets_per_masks_size_per_mask_id_per_reasoning_step[mask_size][mask_id].size();
            compact_posets_per_masks_size_per_mask_id_per_reasoning_step[mask_size][mask_id].push_back(
                    CompactPoset(num_inputs, masks_by_size[mask_size][mask_id], masks_by_size[mask_size][mask_id]));

            CompactPoset *local_compact_poset =
                    &compact_posets_per_masks_size_per_mask_id_per_reasoning_step[mask_size][mask_id][local_compact_poset_id];

            consistent_meta_examples_per_mask_id.push_back(
                    get_meta_example_ids_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
                            masks_by_size[mask_size][mask_id], local_compact_poset));

            masks_ids_by_contained_meta_examples.push_back(
                    make_pair(consistent_meta_examples_per_mask_id[mask_id].size(), mask_id));

            cout << "solved mask = " << bitvector_to_str(masks_by_size[mask_size][mask_id], function_size) << " num_consistent = " << consistent_meta_examples_per_mask_id[mask_id].size() << " time: " << (double) time(nullptr) - local_time << endl;
        }
        sort(masks_ids_by_contained_meta_examples.begin(), masks_ids_by_contained_meta_examples.end());
        reverse(masks_ids_by_contained_meta_examples.begin(), masks_ids_by_contained_meta_examples.end());

        for (int local_id = 0; local_id < masks_ids_by_contained_meta_examples.size(); local_id++) {
            cout << bitvector_to_str(
                    masks_by_size[mask_size][masks_ids_by_contained_meta_examples[local_id].second], function_size)
                 << " num_consistent = " << masks_ids_by_contained_meta_examples[local_id].first << endl;
        }

        for (int local_id = 0; local_id < masks_ids_by_contained_meta_examples.size(); local_id++) {
            cout << "testing " << bitvector_to_str(
                    masks_by_size[mask_size][masks_ids_by_contained_meta_examples[local_id].second], function_size)
                 << " num_consistent = " << masks_ids_by_contained_meta_examples[local_id].first << " time: "
                 << (double) time(nullptr) - local_time << endl;

            int mask_id = masks_ids_by_contained_meta_examples[local_id].second;
            int local_compact_poset_id =
                    (int) compact_posets_per_masks_size_per_mask_id_per_reasoning_step[mask_size][mask_id].size() -
                    1;
            CompactPoset *local_compact_poset =
                    &compact_posets_per_masks_size_per_mask_id_per_reasoning_step[mask_size][mask_id][local_compact_poset_id];

            for (
                    int consistent_meta_example_id = 0;
                    consistent_meta_example_id < consistent_meta_examples_per_mask_id[mask_id].size();
                    consistent_meta_example_id++) {
                int insert_meta_example_id = consistent_meta_examples_per_mask_id[mask_id][consistent_meta_example_id];
                MetaExample local_insert_meta_example =
                        meta_examples[insert_meta_example_id].get_application_of_subdomain(
                                masks_by_size[mask_size][mask_id]);
                if (!local_insert_meta_example.fully_constrained()) {
                    assert(local_compact_poset->insert(local_insert_meta_example));
                }
            }

            bool is_consistent = true;
            for (int test_meta_example_id = 0;
                 test_meta_example_id < meta_examples.size(); test_meta_example_id++) {
                MetaExample local_test_meta_example =
                        meta_examples[test_meta_example_id].get_application_of_subdomain(
                                masks_by_size[mask_size][mask_id]);
                if (!local_test_meta_example.fully_constrained()) {
                    if (!local_compact_poset->insert(local_test_meta_example)) {
                        local_compact_poset->pop();
                        is_consistent = false;
                        break;
                    }
                    local_compact_poset->pop();
                }
            }
            assert(is_consistent);
        }

        int max_num_consistent_meta_examples = masks_ids_by_contained_meta_examples[0].first;


        if(max_num_consistent_meta_examples >=1) {
            vector<MetaExample> meta_examples_with_hints;

            int best_mask_id = masks_ids_by_contained_meta_examples[0].second;

            int best_local_compact_poset_id =
                    (int) compact_posets_per_masks_size_per_mask_id_per_reasoning_step[mask_size][best_mask_id].size() -
                    1;
            CompactPoset *best_local_compact_poset =
                    &compact_posets_per_masks_size_per_mask_id_per_reasoning_step[mask_size][best_mask_id][best_local_compact_poset_id];

            cout << best_local_compact_poset->meta_examples_to_string() << endl;

            int new_bits = 0;
            int expanded_meta_example = 0;

            for (int to_query_meta_example_id = 0;
                 to_query_meta_example_id < meta_examples.size(); to_query_meta_example_id++) {

                MetaExample local_meta_example = meta_examples[to_query_meta_example_id];
                MetaExample local_query_meta_example = local_meta_example.get_application_of_subdomain(
                        masks_by_size[mask_size][best_mask_id]);

                vector<PartialFunction> result = best_local_compact_poset->query(
                        local_query_meta_example.partial_function);

                PartialFunction intermediate_result = result[0];
                for (int k = 1; k < result.size(); k++) {
                    intermediate_result.apply_intersection(result[k]);
                }

                assert((intermediate_result.partition & local_query_meta_example.generalization.partition) ==
                       intermediate_result.partition);

                assert((intermediate_result.partition & local_query_meta_example.partial_function.partition) ==
                       local_query_meta_example.partial_function.partition);

                int local_new_bits = __builtin_popcount(
                        intermediate_result.partition - local_query_meta_example.partial_function.partition);
                expanded_meta_example += (local_new_bits >= 1);
                new_bits += local_new_bits;


                MetaExample meta_example_with_hint = MetaExample(
                        num_inputs,
                        local_meta_example.generalization.total_function,
                        intermediate_result.partition | local_meta_example.partial_function.partition,
                        local_meta_example.generalization.partition,
                        local_meta_example.idx);

                meta_examples_with_hints.push_back(meta_example_with_hint);

                cout << local_query_meta_example.to_string() << " :: " << "{"
                     << meta_examples[to_query_meta_example_id].to_string() << "} -> {"
                     << meta_example_with_hint.to_string() << "} delta = " << local_new_bits << endl;
            }

            cout << "new_bits = " << new_bits << endl;
            cout << "expanded_meta_example = " << expanded_meta_example << endl;

            MinimalFactoringSchema next = MinimalFactoringSchema(num_inputs, meta_examples_with_hints);
        }
    }
}
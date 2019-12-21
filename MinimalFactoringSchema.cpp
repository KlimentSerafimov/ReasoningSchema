//
// Created by Kliment Serafimov on 2019-12-18.
//

#include "MinimalFactoringSchema.h"
#include <iostream>
#include <fstream>
#include <iomanip>

vector<MetaExample> MinimalFactoringSchema::
get_meta_example_ids_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
        int subdomain_mask) {

    vector<MetaExample> consistent_meta_examples;

    for (int insert_meta_example_id = 0; insert_meta_example_id < meta_examples.size(); insert_meta_example_id++) {
//        assert(local_compact_poset->get_num_meta_edges() == 0);
        MetaExample local_insert_meta_example =
                meta_examples[insert_meta_example_id].get_application_of_subdomain(subdomain_mask);
        if (!local_insert_meta_example.fully_constrained()) {
//            local_compact_poset->insert(local_insert_meta_example);
            bool is_consistent = true;
            for (int test_meta_example_id = 0; test_meta_example_id < meta_examples.size(); test_meta_example_id++) {
                MetaExample local_test_meta_example =
                        meta_examples[test_meta_example_id].get_application_of_subdomain(subdomain_mask);
                if (!local_test_meta_example.fully_constrained()) {

                    bool new_implementation_for_consistency =
                            local_insert_meta_example.is_consistent_with(local_test_meta_example);

                    if (!new_implementation_for_consistency) {
                        is_consistent = false;
                        break;
                    }

//                    if (!local_compact_poset->insert(local_test_meta_example)) {
//                        assert(!new_implementation_for_consistency);
//                        local_compact_poset->hard_pop();
//                        is_consistent = false;
//                        break;
//                    }
//                    else
//                    {
//                        if(!new_implementation_for_consistency)
//                        {
//                            cout << local_insert_meta_example.to_string() << endl << local_test_meta_example.to_string() << endl;
//                        }
//                        assert(new_implementation_for_consistency);
//                    }
//                    local_compact_poset->hard_pop();
                }
            }
            if (is_consistent) {
                MetaExample local_consistent_meta_example =
                        meta_examples[insert_meta_example_id].get_application_of_subdomain(subdomain_mask);
                consistent_meta_examples.push_back(local_consistent_meta_example);
            }
//            local_compact_poset->hard_pop();
        }
    }
    return consistent_meta_examples;
}

vector<MetaExample>
MinimalFactoringSchema::get_meta_examples_after_query(int subdomain_mask, CompactPoset *compact_poset, bool print)
{
    vector<MetaExample> meta_examples_with_hints;

    int new_bits = 0;
    int expanded_meta_example = 0;

    for (int to_query_meta_example_id = 0; to_query_meta_example_id < meta_examples.size(); to_query_meta_example_id++) {

        MetaExample local_meta_example = meta_examples[to_query_meta_example_id];
        MetaExample local_query_meta_example = local_meta_example.get_application_of_subdomain(subdomain_mask);

        vector<PartialFunction> result = compact_poset->query(local_query_meta_example.partial_function);

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

        if(print) {
            cout << local_query_meta_example.to_string() << " :: " << "{"
                 << meta_examples[to_query_meta_example_id].to_string() << "} -> {"
                 << meta_example_with_hint.to_string() << "} delta = " << local_new_bits << endl;
        }
    }

    if(print) {
        cout << "new_bits = " << new_bits << endl;
        cout << "expanded_meta_example = " << expanded_meta_example << endl;
    }

    return meta_examples_with_hints;
}


bool MinimalFactoringSchema::test_compact_poset_for_consistency_with_all_meta_examples(
        int subdomain_mask, CompactPoset *compact_poset) {

    bool is_consistent = true;
    for (int test_meta_example_id = 0; test_meta_example_id < meta_examples.size(); test_meta_example_id++) {
        MetaExample local_test_meta_example =
                meta_examples[test_meta_example_id].get_application_of_subdomain(subdomain_mask);
        assert(local_test_meta_example.idx == test_meta_example_id);
        if (!local_test_meta_example.fully_constrained()) {
            if (!compact_poset->insert(local_test_meta_example)) {

                cout << "post insert" << endl;
                compact_poset->print();

                vector<int> meta_example_ids_in_cycle =
                        compact_poset->get_cycle_of_meta_example_ids_and_meta_edge_ids().first;

                compact_poset->hard_pop();

                cout << "CYCLE:" << endl;
                for(int i = 0;i<meta_example_ids_in_cycle.size();i++)
                {
                    cout << meta_example_ids_in_cycle[i] << " ";
                }

                for(int i = 0;i<meta_example_ids_in_cycle.size(); i++)
                {
                    cout << "original: " << meta_examples[meta_example_ids_in_cycle[i]].to_string() << endl;
                    cout << meta_examples[meta_example_ids_in_cycle[i]].get_application_of_subdomain(subdomain_mask).to_string() << endl;
                }

                cout << "NOT CONSISTENT" <<endl;
                is_consistent = false;

                break;
            }
            compact_poset->hard_pop();
        }
    }
    assert(is_consistent);
    return is_consistent;
}

int MinimalFactoringSchema::get_meta_edge_id_to_remove(CompactPoset* compact_poset, int subdomain_mask, int special_meta_example_id)
{
    pair<vector<int>, vector<int> > meta_example_ids_in_cycle =
            compact_poset->get_cycle_of_meta_example_ids_and_meta_edge_ids();

    vector<int> meta_example_ids = meta_example_ids_in_cycle.first;
    vector<int> meta_edges_ids = meta_example_ids_in_cycle.second;

    assert(meta_example_ids.size() == meta_edges_ids.size());

    //find meta_edge that corresponds to meta_example with most specified_bits that is not test_meta_example_id
    pair<int, int> meta_edge_id_to_remove = make_pair(-1, -1);

    for(int i = 0;i<meta_example_ids.size(); i++)
    {
        if(special_meta_example_id != meta_example_ids[i]) {
            int local_meta_example_id = meta_example_ids[i];
            MetaExample local_test_meta_example =
                    meta_examples[local_meta_example_id].get_application_of_subdomain(subdomain_mask);
            meta_edge_id_to_remove = max(
                    meta_edge_id_to_remove,
                    make_pair(__builtin_popcount(local_test_meta_example.partial_function.partition), meta_edges_ids[i]));
        }
    }
    assert(meta_edge_id_to_remove.first != -1);
    return meta_edge_id_to_remove.second;
}

pair<vector<MetaExample>, vector<vector<int> > >  MinimalFactoringSchema::get_globally_consistent_meta_examples_and_equivalent_ids(
        int subdomain_mask, CompactPoset *compact_poset)
{
    for (int test_meta_example_id = 0; test_meta_example_id < meta_examples.size(); test_meta_example_id++) {
        MetaExample local_test_meta_example =
                meta_examples[test_meta_example_id].get_application_of_subdomain(subdomain_mask);
        assert(local_test_meta_example.idx == test_meta_example_id);
        if (!local_test_meta_example.fully_constrained()) {
            if (!compact_poset->insert(local_test_meta_example)) {
                int soft_delete_count = 0;
                do {

                    int meta_edge_id_to_remove =
                            get_meta_edge_id_to_remove(compact_poset, subdomain_mask, test_meta_example_id);

                    compact_poset->hard_pop();

                    soft_delete_count++;
                    cout << "soft_delete_count = " << soft_delete_count << endl;

                    compact_poset->soft_delete(meta_edge_id_to_remove);

                }while(!compact_poset->insert(local_test_meta_example));

                compact_poset->hard_pop();
            }
            compact_poset->hard_pop();
        }
    }

    compact_poset->soft_delete_redundant_edges();

    return compact_poset->get_existant_meta_examples_and_equivalent_ids();

//    vector<int> globally_consistent_meta_example_ids =
//            compact_poset->get_existant_meta_example_ids();
//
//    vector<MetaExample> globally_consistent_meta_examples;
//
//    for(int i = 0;i<globally_consistent_meta_example_ids.size();i++)
//    {
//        globally_consistent_meta_examples.push_back(meta_examples[globally_consistent_meta_example_ids[i]]);
//    }
//
//    return globally_consistent_meta_examples;
}

int get_num_missing_bits(vector<MetaExample> meta_examples)
{
    int ret = 0;
    for(int i = 0; i<meta_examples.size();i++)
    {
        ret += __builtin_popcount(meta_examples[i].generalization.partition) -
                __builtin_popcount(meta_examples[i].partial_function.partition);
    }
    return ret;
}

pair<double, pair<int, vector<int> > > MinimalFactoringSchema::calculate_heuristic(int current_number_missing_bits, int mask_size, int mask_id) {
    auto count_occurences =
            vector<vector<pair<int, pair<int, int> > > >(meta_examples.size(), vector<pair<int, pair<int, int> > >());

    vector<vector<vector<int> > > covered;

    int at_mask_size = mask_size;
    int at_mask_id = mask_id;
    MinimalFactoringSchema* at_parent = this;

    int very_init_num_missing_bits = -1;

    while(at_parent != nullptr)
    {
        very_init_num_missing_bits = at_parent->init_num_missing_bits;
        covered.push_back(vector<vector<int> >());
        for(int i = 0;i<at_parent->equivalent_meta_example_ids[at_mask_size][at_mask_id].size(); i++)
        {
            covered[covered.size()-1].push_back(vector<int>());
            for(int j = 0;j<at_parent->equivalent_meta_example_ids[at_mask_size][at_mask_id][i].size(); j++)
            {
                count_occurences[at_parent->equivalent_meta_example_ids[at_mask_size][at_mask_id][i][j]].push_back(
                        make_pair(covered.size()-1, make_pair(i, j)));
//                cout << at_parent->equivalent_meta_example_ids[at_mask_size][at_mask_id][i][j] <<" ";
            }
//            cout << endl;
        }
//        cout << endl;
        at_parent = at_parent->parent_pointer;
        if(at_parent != nullptr)
        {
            at_mask_size = at_parent->best_mask_size;
            at_mask_id = at_parent->best_mask_id;
        }
    }

    vector<pair<int, int> > meta_examples_id_by_occurences;

    for(int i = 0;i<count_occurences.size();i++)
    {
        meta_examples_id_by_occurences.push_back(make_pair(count_occurences[i].size(), i));
    }

    sort(meta_examples_id_by_occurences.begin(), meta_examples_id_by_occurences.end());
    reverse(meta_examples_id_by_occurences.begin(), meta_examples_id_by_occurences.end());


    int num_necessary_meta_examples = 0;
    vector<int> necessary_meta_examples;

    for(int i = 0;i<meta_examples_id_by_occurences.size();i++)
    {
        int id = meta_examples_id_by_occurences[i].second;
//        cout << "id = " << id << endl;
        bool necessary = false;
        for(int j = 0;j<count_occurences[id].size();j++)
        {
            int parent_id = count_occurences[id][j].first;
            int sub_meta_example_id = count_occurences[id][j].second.first;
//            cout << parent_id <<" "<< sub_meta_example_id << " | ";
            if(covered[parent_id][sub_meta_example_id].size() == 0)
            {
                necessary = true;
                break;
            }
        }
//        cout << endl;

        if(necessary)
        {
            num_necessary_meta_examples++;
            necessary_meta_examples.push_back(id);

            for(int j = 0;j<count_occurences[id].size();j++)
            {
                int parent_id = count_occurences[id][j].first;
                int sub_meta_example_id = count_occurences[id][j].second.first;
                covered[parent_id][sub_meta_example_id].push_back(id);
            }
        }
    }



    int total_delta_num_bits = very_init_num_missing_bits - current_number_missing_bits;

    int current_delta = init_num_missing_bits - current_number_missing_bits;

    if(num_necessary_meta_examples == 0)
    {
        return make_pair(-1, make_pair(total_delta_num_bits, necessary_meta_examples));
    }

    if(current_number_missing_bits == init_num_missing_bits)
    {
        return make_pair(-1, make_pair(total_delta_num_bits, necessary_meta_examples));
    }

    int prev_necessary_meta_examples_delta = 0;
    if(parent_pointer != nullptr)
    {
        prev_necessary_meta_examples_delta = parent_pointer->best_necessary_num_meta_examples;
    }
    int delta_necessary_meta_examples_delta = num_necessary_meta_examples - prev_necessary_meta_examples_delta;

    double ret;
    if(delta_necessary_meta_examples_delta == 0)
    {
        ret = 1000000 + current_delta;
    }
    else
    {
        ret = (double) current_delta / (double) delta_necessary_meta_examples_delta;
    }


//    double ret = (double) total_delta_num_bits / (double) num_necessary_meta_examples;


    return make_pair(ret, make_pair(total_delta_num_bits, necessary_meta_examples));
}

static int minimal_factoring_schema_depth_counter = 0;

static string best_subdomains[30] =
        {
                "10101000",
                "01100011",
                "01001011",
                "10100010",
                "11010100",

                "00101110",
                "00110110",
                "10100110",
                "11001000",
                "01010101",

                "10001010",
                "10001110",
                "10010011",
                "11100000",
                "11101000",

                "10111000",
                "00001111",
                "01100011",
                "01011010",
                "01010100",

                "01110010",
                "11010100",
                "10001101",
                "00100111",
                "01110001",

                "10011010",
                "01011010",
                "10011101",
                "10101000",
                "01100010"
        };

//static ofstream fout("subdomain_masks_4_5_6_7_div_4_running.out");
//static ofstream fout("subdomain_masks_num_bits_2_running.out");
static ofstream fout("fast_subdomain_masks_5_6_7_running_delta_heuristic.out");
//static ofstream fout("fast_new_subdomain_masks_1_2_3_4_5_6_7_running_delta_heuristic.out");
//static ofstream fout("testing_consistency.out");

MinimalFactoringSchema::MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples, MinimalFactoringSchema *_parent_pointer) {
    parent_pointer = _parent_pointer;

    if(parent_pointer == nullptr)
    {
        minimal_factoring_schema_depth_counter = 0;
    }

    time_t local_time = (double) time(nullptr);

    num_inputs = _num_inputs;
    function_size = (1 << num_inputs);
    meta_examples = _meta_examples;
    vector<vector<int> > masks_by_size = vector<vector<int> >(function_size + 1, vector<int>());

    vector<int> unit_masks;
    vector<CompactPoset> compact_posets;

    for (int i = 1; i < (1 << function_size)-1; i++) {
        if(minimal_factoring_schema_depth_counter < 0) {
            i = string_to_bitvector(best_subdomains[minimal_factoring_schema_depth_counter++]);
            masks_by_size[__builtin_popcount(i)].push_back(i);
            break;
        } else
        {
            masks_by_size[__builtin_popcount(i)].push_back(i);
        }
    }

    for(int i = 0;i<masks_by_size.size();i++)
    {
        vector<int> reduced;
        for(int j = 0;j<masks_by_size[i].size();j++)
        {
            reduced.push_back(masks_by_size[i][j]);
        }
        masks_by_size[i] = reduced;
    }

    init_num_missing_bits = get_num_missing_bits(meta_examples);

    fout << init_num_missing_bits << endl;

    vector<pair<double, pair<int, int> > > mask_size_and_masks_ids_by_heuristic;
    vector<vector<vector<int> > > necessary_meta_examples_per_mask_size_per_mask_id;
    vector<vector<int> > total_delta_num_missing_bits;

    globally_consistent_meta_examples =
            vector<vector<vector<MetaExample> > >(function_size+1, vector<vector<MetaExample> >());
    equivalent_meta_example_ids =
            vector<vector<vector<vector<int> > > >(function_size+1, vector<vector<vector<int> > >());

    necessary_meta_examples_per_mask_size_per_mask_id =
            vector<vector<vector<int> > >(function_size+1, vector<vector<int> >());
    total_delta_num_missing_bits =
            vector<vector<int> >(function_size+1, vector<int>());

    for (int mask_size = 5; mask_size < masks_by_size.size(); mask_size++) {
        cout << "working on masks_size = " << mask_size << endl;
        for (int mask_id = 0; mask_id < masks_by_size[mask_size].size(); mask_id++) {

            int subdomain_mask = masks_by_size[mask_size][mask_id];
            cout << "working on mask = " << bitvector_to_str(subdomain_mask, function_size) << " time: " << (double) time(nullptr) - local_time << endl;

            vector<MetaExample> individually_consistent_meta_examples =
                    get_meta_example_ids_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
                            subdomain_mask);

            cout << "num_individually_consistent_for_rest = " << individually_consistent_meta_examples.size() << endl;

            CompactPoset *local_compact_poset =
                    new CompactPoset(num_inputs, subdomain_mask, subdomain_mask, individually_consistent_meta_examples);

            cout << "num_individually_consistent_together_in_compact_poset = " << individually_consistent_meta_examples.size() << endl;

            pair<vector<MetaExample>, vector<vector<int> > > globally_consistent_meta_examples_and_equivalent_ids =
                    get_globally_consistent_meta_examples_and_equivalent_ids(subdomain_mask, local_compact_poset);

            cout << "num_individually_consistent_together_in_compact_poset_after_remove_redundant_for_rest = " << globally_consistent_meta_examples_and_equivalent_ids.first.size() << endl;

            local_compact_poset =
                    new CompactPoset(
                            num_inputs, subdomain_mask, subdomain_mask,
                            globally_consistent_meta_examples_and_equivalent_ids.first);

            test_compact_poset_for_consistency_with_all_meta_examples(subdomain_mask, local_compact_poset);

            cout << "tested mask = " << bitvector_to_str(subdomain_mask, function_size) << " time: " << (double) time(nullptr) - local_time << endl;

            vector<MetaExample> meta_examples_after_query =
                    get_meta_examples_after_query(subdomain_mask, local_compact_poset, false);

            int num_missing_bits = get_num_missing_bits(meta_examples_after_query);
            int delta_num_bits = init_num_missing_bits - num_missing_bits;

            assert(globally_consistent_meta_examples[mask_size].size() == equivalent_meta_example_ids[mask_size].size());
            assert(globally_consistent_meta_examples[mask_size].size() == mask_id);

            assert(globally_consistent_meta_examples_and_equivalent_ids.first.size() == globally_consistent_meta_examples_and_equivalent_ids.second.size());

            globally_consistent_meta_examples[mask_size].push_back(
                    globally_consistent_meta_examples_and_equivalent_ids.first);
            equivalent_meta_example_ids[mask_size].push_back(
                    globally_consistent_meta_examples_and_equivalent_ids.second);

            pair<double, pair<int, vector<int> > > heuristic = calculate_heuristic(num_missing_bits, mask_size, mask_id);
            necessary_meta_examples_per_mask_size_per_mask_id[mask_size].push_back(heuristic.second.second);
            total_delta_num_missing_bits[mask_size].push_back(heuristic.second.first);

            mask_size_and_masks_ids_by_heuristic.push_back(
                    make_pair(heuristic.first, make_pair(mask_size, mask_id)));

            cout << "heuristic = " << fixed << setprecision(4) << heuristic.first << endl;
            cout << "delta_num_bits = " << delta_num_bits << endl;
            cout << "total_delta_num_bits = " << heuristic.second.first << endl;
            cout << "total_num_necessary_meta_examples = " << heuristic.second.second.size() << endl;
            cout << "remaining_num_bits = " << num_missing_bits << endl;
            cout << "solved mask = " << bitvector_to_str(subdomain_mask, function_size) << " time: " << (double) time(nullptr) - local_time << endl;
            cout << endl;
        }
    }

    sort(mask_size_and_masks_ids_by_heuristic.begin(), mask_size_and_masks_ids_by_heuristic.end());
    reverse(mask_size_and_masks_ids_by_heuristic.begin(), mask_size_and_masks_ids_by_heuristic.end());

    for (int local_id = 0; local_id < mask_size_and_masks_ids_by_heuristic.size(); local_id++) {
        int mask_size = mask_size_and_masks_ids_by_heuristic[local_id].second.first;
        int mask_id = mask_size_and_masks_ids_by_heuristic[local_id].second.second;

        cout << bitvector_to_str(masks_by_size[mask_size][mask_id], function_size) << fixed << setprecision(6)
             << " heuristic_measure = " << mask_size_and_masks_ids_by_heuristic[local_id].first << " delta_num_missing_bits = "
             << total_delta_num_missing_bits[mask_size][mask_id] <<" total_num_necessary_meta_examples = "
             <<  necessary_meta_examples_per_mask_size_per_mask_id[mask_size][mask_id].size() << endl;
    }

    double max_num_consistent_meta_examples = mask_size_and_masks_ids_by_heuristic[0].first;

    if(max_num_consistent_meta_examples >= 0.00001) {

        best_mask_size = mask_size_and_masks_ids_by_heuristic[0].second.first;
        best_mask_id = mask_size_and_masks_ids_by_heuristic[0].second.second;
        int subdomain_mask = masks_by_size[best_mask_size][best_mask_id];

        fout << bitvector_to_str(subdomain_mask, function_size) <<" ";

        CompactPoset *best_local_compact_poset =
                new CompactPoset(
                        num_inputs, subdomain_mask, subdomain_mask,
                        globally_consistent_meta_examples[best_mask_size][best_mask_id]);

        best_necessary_num_meta_examples =
                necessary_meta_examples_per_mask_size_per_mask_id[best_mask_size][best_mask_id].size();

        fout <<  best_necessary_num_meta_examples << " ";

        best_local_compact_poset->soft_delete_redundant_edges();

        cout << "num_meta_edges: " << best_local_compact_poset->get_num_meta_edges() <<" ";
        cout << best_local_compact_poset->meta_examples_to_string() << endl;

        vector<MetaExample> meta_examples_with_hints = get_meta_examples_after_query(
                masks_by_size[best_mask_size][best_mask_id], best_local_compact_poset, true);

        MinimalFactoringSchema next = MinimalFactoringSchema(num_inputs, meta_examples_with_hints, this);
    }
}


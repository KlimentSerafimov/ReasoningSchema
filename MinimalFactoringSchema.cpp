//
// Created by Kliment Serafimov on 2019-12-18.
//

#include "MinimalFactoringSchema.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

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

vector<MetaExample> get_meta_examples_after_query(int subdomain_mask, CompactPoset *compact_poset, vector<MetaExample> meta_examples,
                                                  bool print, bool query_only_active, bool carry_over_active)
{
    vector<MetaExample> meta_examples_with_hints;

    int new_bits = 0;
    int expanded_meta_example = 0;

    for (int to_query_meta_example_id = 0; to_query_meta_example_id < meta_examples.size(); to_query_meta_example_id++) {
//
//        if(!query_only_active)
//        {
//            meta_examples[to_query_meta_example_id].active = true;
//        }

        MetaExample local_meta_example = meta_examples[to_query_meta_example_id];
        MetaExample local_query_meta_example = local_meta_example.get_application_of_subdomain(subdomain_mask);

//        if(query_only_active)
//        {
//            if(!local_query_meta_example.active)
//            {
//                meta_examples_with_hints.push_back(local_meta_example);
//                continue;
//            }
//        }

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
                local_meta_example.generalization.num_inputs,
                local_meta_example.generalization.total_function,
                local_meta_example.partial_function.partition | intermediate_result.partition,
                local_meta_example.generalization.partition,
                local_meta_example.idx,
                (local_new_bits >= 1) || (carry_over_active && local_meta_example.active)
                );

        meta_examples_with_hints.push_back(meta_example_with_hint);

        if(print) {
            cout << local_query_meta_example.to_string() << " :: " << "{"
                 << meta_examples[to_query_meta_example_id].to_string() << "} -> {"
                 << meta_example_with_hint.to_string() << "} delta = " << local_new_bits << endl;
        }
    }

    if(print) {
        cout << "new_bits = " << new_bits << endl;
        cout << "expanded_meta_examples = " << expanded_meta_example << endl;
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

void MinimalFactoringSchema::prune_globally_inconsistent_meta_examples(
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

double MinimalFactoringSchema::calculate_heuristic(Module* module) {

    auto count_occurences =
            vector<vector<pair<int, pair<int, int> > > >
                    (meta_examples.size(), vector<pair<int, pair<int, int> > >());

    MinimalFactoringSchema* at_parent = this;

    int very_init_num_missing_bits = -1;

    very_init_num_missing_bits = at_parent->init_num_missing_bits;
    module->covered.push_back(vector<vector<int> >());
    for(int i = 0;i<module->equivalent_ids.size(); i++)
    {
        module->covered[module->covered.size()-1].push_back(vector<int>());
        for(int j = 0;j<module->equivalent_ids[i].size(); j++)
        {
            count_occurences[module->equivalent_ids[i][j]].push_back(
                    make_pair(module->covered.size()-1, make_pair(i, j)));
//                cout << at_parent->equivalent_meta_example_ids[at_mask_size][at_mask_id][i][j] <<" ";
        }
//            cout << endl;
    }
//        cout << endl;
    at_parent = at_parent->parent_pointer;

    while(at_parent != nullptr)
    {
        very_init_num_missing_bits = at_parent->init_num_missing_bits;
        module->covered.push_back(vector<vector<int> >());
        for(int i = 0;i<at_parent->best_module.equivalent_ids.size(); i++)
        {
            module->covered[module->covered.size()-1].push_back(vector<int>());
            for(int j = 0;j<at_parent->best_module.equivalent_ids[i].size(); j++)
            {
                count_occurences[at_parent->best_module.equivalent_ids[i][j]].push_back(
                        make_pair(module->covered.size()-1, make_pair(i, j)));
//                cout << at_parent->equivalent_meta_example_ids[at_mask_size][at_mask_id][i][j] <<" ";
            }
//            cout << endl;
        }
//        cout << endl;
        at_parent = at_parent->parent_pointer;
    }

    vector<pair<int, int> > meta_examples_id_by_occurences;

    for(int i = 0;i<count_occurences.size();i++)
    {
        meta_examples_id_by_occurences.push_back(make_pair(count_occurences[i].size(), i));
    }

    sort(meta_examples_id_by_occurences.begin(), meta_examples_id_by_occurences.end());
    reverse(meta_examples_id_by_occurences.begin(), meta_examples_id_by_occurences.end());

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
            if(module->covered[parent_id][sub_meta_example_id].size() == 0)
            {
                necessary = true;
                break;
            }
        }
//        cout << endl;

        if(necessary)
        {
            module->necessary_meta_example_ids.push_back(id);

            for(int j = 0;j<count_occurences[id].size();j++)
            {
                int parent_id = count_occurences[id][j].first;
                int sub_meta_example_id = count_occurences[id][j].second.first;
                module->covered[parent_id][sub_meta_example_id].push_back(id);
            }
        }
    }


    int num_necessary_meta_examples = (int) module->necessary_meta_example_ids.size();


    int total_delta_num_bits = very_init_num_missing_bits - module->num_missing_bits;

//    cout << "HERE: init_num_missing_bits" << init_num_missing_bits <<" module->num_missing_bits = "<< module->num_missing_bits << endl;

    int current_delta = init_num_missing_bits - module->num_missing_bits;

    if(num_necessary_meta_examples == 0)
    {
        return -1;
    }

    if(module->num_missing_bits == init_num_missing_bits)
    {
        return -1;
    }

    int prev_necessary_meta_examples_delta = 0;
    if(parent_pointer != nullptr)
    {
        prev_necessary_meta_examples_delta = (int) parent_pointer->best_module.necessary_meta_example_ids.size();
    }
    int delta_num_necessary_meta_examples = num_necessary_meta_examples - prev_necessary_meta_examples_delta;

    double delta_ratio;

    cout << "num_necessary_meta_examples = " << num_necessary_meta_examples << " delta_num_necessary_meta_examples = "<< delta_num_necessary_meta_examples << endl;

    if(delta_num_necessary_meta_examples == 0)
    {
        delta_ratio = 1000000 + current_delta;
    }
    else
    {
        delta_ratio = (double) current_delta / (double) delta_num_necessary_meta_examples;
    }


//    double ret = (double) total_delta_num_bits / (double) num_necessary_meta_examples;

//    cout << "C" << endl;

    return delta_ratio;
}


static int maximal_factoring_schema_depth_counter = 0;
static int minimal_factoring_schema_depth_counter = 0;

static string best_subdomains[1000] = {};

vector<int> MinimalFactoringSchema::get_masks(int set_init_mask_size)
{
    vector<int> masks;
    vector<vector<int> > masks_by_size = vector<vector<int> >(function_size + 1, vector<int>());

    for (int i = 1; i < (1 << function_size)-1; i++) {
        if(minimal_factoring_schema_depth_counter < maximal_factoring_schema_depth_counter) {
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
        if(i >= set_init_mask_size) {
            for (int j = 0; j < masks_by_size[i].size(); j++) {
                reduced.push_back(masks_by_size[i][j]);
                masks.push_back(masks_by_size[i][j]);
            }
        }
        masks_by_size[i] = reduced;
    }
    return masks;
}

static ofstream fout;

void MinimalFactoringSchema::repeat_apply_parents(Module *module) {

    if(parent_pointer == nullptr)
    {
        return;
    }

//    int after_cycle_num_missig_bits = get_num_missing_bits(meta_examples_with_hints);//local_after_num_missig_bits;

    vector<MetaExample> inactive_meta_examples;
    vector<MetaExample> active_meta_examples;

    for(int i = 0;i<module->meta_examples_after_query.size();i++)
    {
        if(module->meta_examples_after_query[i].active)
        {
            active_meta_examples.push_back(module->meta_examples_after_query[i]);
        }
        else
        {
            inactive_meta_examples.push_back(module->meta_examples_after_query[i]);
        }
    }

//    active_meta_examples = meta_examples;
//    inactive_meta_examples = vector<MetaExample>();

    int num_missing_bits_in_active = get_num_missing_bits(active_meta_examples);
    int num_missing_bits_in_inactive = get_num_missing_bits(inactive_meta_examples);

    int after_cycle_num_missig_bits = num_missing_bits_in_active;

    vector<MinimalFactoringSchema*> parents;
    vector<CompactPoset*> compact_posets;
    vector<int> best_subdomain_masks;

    MinimalFactoringSchema* at_parent = this;

    while(at_parent != nullptr)
    {
        parents.push_back(at_parent);
        if(at_parent == this)
        {
            compact_posets.push_back(module->compact_poset);
            best_subdomain_masks.push_back(module->subdomain_mask);

        } else
        {
            compact_posets.push_back(at_parent->best_module.compact_poset);
            best_subdomain_masks.push_back(at_parent->best_module.subdomain_mask);
        }
        at_parent = at_parent->parent_pointer;
    }

    reverse(parents.begin(), parents.end());
    reverse(compact_posets.begin(), compact_posets.end());
    reverse(best_subdomain_masks.begin(), best_subdomain_masks.end());

    int local_repeat_count = 0;

    int cycle_prev_num_missing_bit = -1;

    do{
        local_repeat_count += 1;
        cycle_prev_num_missing_bit = after_cycle_num_missig_bits;
        int prev_for_loop_num_missing_bits = after_cycle_num_missig_bits;
        for (int i = 0; i < compact_posets.size(); i++) {

            active_meta_examples = get_meta_examples_after_query(
                    best_subdomain_masks[i], compact_posets[i], active_meta_examples, false, true, true);

            int local_num_missing_bits = get_num_missing_bits(active_meta_examples);

//            cout << "at i = "<< i <<" local_num_missing_bits = " << local_num_missing_bits << endl;

            if(local_num_missing_bits < prev_for_loop_num_missing_bits) {
//                cout << "to repeat i = " << i << endl;
                module->repeats.push_back(make_pair(i, &parents[i]->best_module));
//              num_missing_bits_in_inactive = 0;
                module->num_missing_bits_after_repeat.push_back(num_missing_bits_in_inactive+local_num_missing_bits);
            }
            prev_for_loop_num_missing_bits = local_num_missing_bits;

//            cout << "after i = " << i <<" num_missing_bits = " << local_num_missing_bits << endl;
        }
        after_cycle_num_missig_bits = prev_for_loop_num_missing_bits;
    }while(after_cycle_num_missig_bits != cycle_prev_num_missing_bit);


    for(int i = 0, j = 0, k = 0; i<module->meta_examples_after_query.size();i++)
    {
        bool enter = false;
        if(j < active_meta_examples.size()) {
            if (active_meta_examples[j].idx == module->meta_examples_after_query[i].idx) {
                enter = true;
                module->meta_examples_after_query[i] = active_meta_examples[j];
                j++;
            }
        }
        if(k < inactive_meta_examples.size()) {
            if (inactive_meta_examples[k].idx == module->meta_examples_after_query[i].idx) {
                assert(!enter);
                enter = true;
                module->meta_examples_after_query[i] = inactive_meta_examples[k];
                k++;
            }
        }
        assert(enter);
    }

}

//static ofstream fout("subdomain_masks_4_5_6_7_div_4_running.out");
//static ofstream fout("subdomain_masks_num_bits_2_running.out");
//static ofstream fout("fast_subdomain_masks_5_6_7_running_delta_heuristic.out");
//static ofstream fout("fast_new_new_new_subdomain_masks_1_2_3_4_5_6_7_running_delta_heuristic.out");

//static ofstream fout("fast_subdomain_masks_num_bits_2_with_repeats_traced_delta_heuristic.out");
//static ofstream fout("testing.out");

static int set_init_mask_size = 3;
//static string fout_name = "fast_subdomain_masks_3_4_5_6_7_with_repeats_traced_corrected_only_active_eval_delta_heuristic.out";
static string fout_name = "after_refactor__reasoning_schema_using_delta_heuristic__subdomain_size_3plus.out";

MinimalFactoringSchema::MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples, string ordering_name) {
    parent_pointer = nullptr;

    assert(!fout.is_open());

    fout.open(ordering_name + "__" + fout_name);

    bool skip = false;
    if (!skip) {
        minimal_factoring_schema_depth_counter = 0;

        ifstream fin("fast_subdomain_masks_3_4_5_6_7_with_repeats_traced_corrected_only_active_eval_delta_heuristic.out");

        string line;
        int line_id = 0;
        cout << "Already processed: " << endl;
        while (getline(fin, line)) {
            stringstream ss;
            ss << line;
            string first;
            if (line_id == 0) {
                ss >> first;
                cout << first << endl;
            } else {
                string second, third, fourth;
                ss >> first >> second >> third >> fourth;
                assert(maximal_factoring_schema_depth_counter < 1000);
                cout << first << " " << second << " " << third << " " << fourth << endl;
                if (fourth != "mask") {
                    best_subdomains[maximal_factoring_schema_depth_counter++] = first;
                }
            }
            line_id++;
        }
    }

    main__minimal_factoring_schema(_num_inputs, _meta_examples);

    fout.close();
}

MinimalFactoringSchema::MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples,
            MinimalFactoringSchema *_parent_pointer) {

    parent_pointer = _parent_pointer;
    main__minimal_factoring_schema(_num_inputs, _meta_examples);
}

void MinimalFactoringSchema::calc_module(int subdomain_mask, Module* module, bool print)
{

    module->function_size = function_size;
    module->subdomain_mask = subdomain_mask;

    if(parent_pointer == nullptr)
    {
        module->parent_module = nullptr;
    }
    else
    {
        module->parent_module = &parent_pointer->best_module;
    }

    vector<MetaExample> individually_consistent_meta_examples =
            get_meta_example_ids_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
                    subdomain_mask);

//    cout << "num_individually_consistent_for_rest = " << individually_consistent_meta_examples.size() << endl;

    module->compact_poset =
            new CompactPoset(num_inputs, subdomain_mask, subdomain_mask, individually_consistent_meta_examples);

//    cout << "num_individually_consistent_together_in_compact_poset = " << individually_consistent_meta_examples.size() << endl;

    prune_globally_inconsistent_meta_examples(subdomain_mask, module->compact_poset);

    module->compact_poset->soft_delete_redundant_edges();

    module->compact_poset->get_existant_meta_examples_and_equivalent_ids(
            module->module_meta_examples, module->equivalent_ids);

//    cout << "num_individually_consistent_together_in_compact_poset_after_remove_redundant_for_rest = " << module->module_meta_examples.size() << endl;

//    module->compact_poset->clear();
//
//    module->compact_poset =
//            new CompactPoset(
//                    num_inputs, subdomain_mask, subdomain_mask,
//                    module->module_meta_examples);

    test_compact_poset_for_consistency_with_all_meta_examples(subdomain_mask, module->compact_poset);

//    cout << "tested mask = " << bitvector_to_str(subdomain_mask, function_size) << " time: " << (double) time(nullptr) - local_time << endl;

    module->meta_examples_after_query =
            get_meta_examples_after_query(subdomain_mask, module->compact_poset, meta_examples, false, false, false);

    module->intermediate_num_missing_bits = get_num_missing_bits(module->meta_examples_after_query);
    int intermediate_delta_num_bits = init_num_missing_bits - module->intermediate_num_missing_bits;
    cout << "intermediate_num_missing_bits = " << module->intermediate_num_missing_bits << " intermediate_delta_num_bits = " << intermediate_delta_num_bits << endl;

    repeat_apply_parents(module);

    module->num_missing_bits = get_num_missing_bits(module->meta_examples_after_query);
    int delta_num_bits = init_num_missing_bits - module->num_missing_bits;
    cout << "after_repeat_num_missing_bits = " << module->num_missing_bits << " after_repeat_delta_num_missing_bits = " << delta_num_bits << endl;


}

void MinimalFactoringSchema::main__minimal_factoring_schema(int _num_inputs, vector<MetaExample> _meta_examples) {

    num_inputs = _num_inputs;
    function_size = (1 << num_inputs);
    meta_examples = _meta_examples;

    local_time = (double) time(nullptr);

    if (parent_pointer == nullptr) {
        init_time = local_time;
        root_pointer = this;
    } else {
        init_time = parent_pointer->init_time;
        root_pointer = parent_pointer->root_pointer;
    }

    vector<int> masks = get_masks(set_init_mask_size);

    init_num_missing_bits = get_num_missing_bits(meta_examples);

    vector<pair<double, int> > mask_ids_by_heuristic;

    cout << "init get__global_num_decision_tree_nodes() = " << get__global_num_decision_tree_nodes() << " get__empty_slots_count() = "<< get__empty_slots_count() << endl;

    for(int mask_id = 0; mask_id < masks.size(); mask_id++)
    {
        Module local_module;

        cout << "working on mask = " << bitvector_to_str(masks[mask_id], function_size) << " time: " << (double) time(nullptr) - local_time << endl;

        calc_module(masks[mask_id], &local_module, false);

        local_module.compact_poset->clear();

        double heuristic = calculate_heuristic(&local_module);

        mask_ids_by_heuristic.push_back(make_pair(heuristic, mask_id));

        cout << "heuristic = " << fixed << setprecision(4) << heuristic << endl;
    }

    sort(mask_ids_by_heuristic.begin(), mask_ids_by_heuristic.end());
    reverse(mask_ids_by_heuristic.begin(), mask_ids_by_heuristic.end());

    for (int local_id = 0; local_id < mask_ids_by_heuristic.size(); local_id++) {
        int mask_id = mask_ids_by_heuristic[local_id].second;
        cout << bitvector_to_str(masks[mask_id], function_size) << fixed << setprecision(6)
             << " heuristic_measure = " << mask_ids_by_heuristic[local_id].first << endl;
    }

    double max_num_consistent_meta_examples = mask_ids_by_heuristic[0].first;

    if(max_num_consistent_meta_examples >= 0.00001) {

        if(parent_pointer == nullptr)
        {
            fout << init_num_missing_bits << endl;
        }

        int best_mask_id = mask_ids_by_heuristic[0].second;

        calc_module(masks[best_mask_id], &best_module, true);

        calculate_heuristic(&best_module);

        fout << best_module.print_module_sketch(time(nullptr) - init_time);

        MinimalFactoringSchema next = MinimalFactoringSchema(num_inputs, best_module.meta_examples_after_query, this);
    }
    else 
    {
        fout << parent_pointer->best_module.covered_to_string(root_pointer->meta_examples) << endl;
    }
}

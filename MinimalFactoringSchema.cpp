//
// Created by Kliment Serafimov on 2019-12-18.
//

#include "MinimalFactoringSchema.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

vector<MetaExample> get_meta_examples_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
        int subdomain_mask, vector<MetaExample> meta_examples)
{
    vector<MetaExample> consistent_meta_examples;

    for (int insert_meta_example_id = 0; insert_meta_example_id < meta_examples.size(); insert_meta_example_id++)
    {
        MetaExample local_insert_meta_example =
                meta_examples[insert_meta_example_id].get_application_of_subdomain(subdomain_mask);
        if (!local_insert_meta_example.fully_constrained())
        {
            bool is_consistent = true;
            for (int test_meta_example_id = 0; test_meta_example_id < meta_examples.size(); test_meta_example_id++)
            {
                MetaExample local_test_meta_example =
                        meta_examples[test_meta_example_id].get_application_of_subdomain(subdomain_mask);
                if (!local_test_meta_example.fully_constrained())
                {
                    bool new_implementation_for_consistency =
                            local_insert_meta_example.is_consistent_with(local_test_meta_example);

                    if (!new_implementation_for_consistency)
                    {
                        is_consistent = false;
                        break;
                    }
                }
            }
            if (is_consistent)
            {
//                MetaExample local_consistent_meta_example =
//                        meta_examples[insert_meta_example_id].get_application_of_subdomain(subdomain_mask);
                consistent_meta_examples.push_back(local_insert_meta_example);
            }
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
            intermediate_result.apply_common_partition(result[k]);
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
                local_meta_example.generalization.function_size,
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
                module->repeats_module_pointers.push_back(&parents[i]->best_module);
                module->repeats_module_ids.push_back(i);
//              num_missing_bits_in_inactive = 0;
                module->num_missing_bits_per_repeat.push_back(num_missing_bits_in_inactive+local_num_missing_bits);
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

HeuristicScore MinimalFactoringSchema::calculate_heuristic(Module* module) {

    if(parent_pointer == nullptr)
    {
        module->count_occurences =
                vector<vector<pair<int, pair<int, int> > > >
                        (meta_examples.size(), vector<pair<int, pair<int, int> > >());
        module->covering = vector<vector<vector<int> > >();
    }
    else
    {
        module->count_occurences = parent_pointer->best_module.count_occurences;
        module->covering = vector<vector<vector<int> > >(
                parent_pointer->best_module.covering.size(), vector<vector<int> >());

    }
    int module_id = (int) module->covering.size();
    module->covering.push_back(vector<vector<int> >(module->equivalent_ids.size(), vector<int>()));

    for(int i = 0;i<module->equivalent_ids.size(); i++)
    {
        for (int j = 0; j < module->equivalent_ids[i].size(); j++) {
            module->count_occurences[module->equivalent_ids[i][j]].push_back(
                    make_pair(module_id, make_pair(i, j)));
        }
    }

    MinimalFactoringSchema* at_parent = this;
    at_parent = at_parent->parent_pointer;

    while(at_parent != nullptr)
    {
        module_id--;
        module->covering[module_id] = vector<vector<int> >(
                at_parent->best_module.equivalent_ids.size(), vector<int>());
        at_parent = at_parent->parent_pointer;
    }

    vector<pair<int, int> > meta_examples_id_by_occurences;

    for(int i = 0;i<module->count_occurences.size();i++)
    {
        meta_examples_id_by_occurences.push_back(make_pair(module->count_occurences[i].size(), i));
    }

    sort(meta_examples_id_by_occurences.begin(), meta_examples_id_by_occurences.end());
    reverse(meta_examples_id_by_occurences.begin(), meta_examples_id_by_occurences.end());

    for(int i = 0;i<meta_examples_id_by_occurences.size();i++)
    {
        int id = meta_examples_id_by_occurences[i].second;
//        cout << "id = " << id << endl;
        bool necessary = false;
        for(int j = 0;j<module->count_occurences[id].size();j++)
        {
            int parent_id = module->count_occurences[id][j].first;
            int sub_meta_example_id = module->count_occurences[id][j].second.first;
//            cout << parent_id <<" "<< sub_meta_example_id << " | ";
            if(module->covering[parent_id][sub_meta_example_id].size() == 0)
            {
                necessary = true;
                break;
            }
        }
//        cout << endl;

        if(necessary)
        {
            module->necessary_meta_example_ids.push_back(id);

            for(int j = 0;j<module->count_occurences[id].size();j++)
            {
                int parent_id = module->count_occurences[id][j].first;
                int sub_meta_example_id = module->count_occurences[id][j].second.first;
                module->covering[parent_id][sub_meta_example_id].push_back(id);
            }
        }
    }

    int num_necessary_meta_examples = (int) module->necessary_meta_example_ids.size();

//    cout << "HERE: init_num_missing_bits" << init_num_missing_bits <<" module->num_missing_bits = "<< module->num_missing_bits << endl;

    if(num_necessary_meta_examples == 0)
    {
        return HeuristicScore();
    }

    if(module->num_missing_bits == init_num_missing_bits)
    {
        return HeuristicScore();
    }

    int prev_necessary_meta_examples_delta = 0;
    if(parent_pointer != nullptr)
    {
        prev_necessary_meta_examples_delta = (int) parent_pointer->best_module.necessary_meta_example_ids.size();
    }

    int delta_num_necessary_meta_examples = num_necessary_meta_examples - prev_necessary_meta_examples_delta;

//    cout << "num_necessary_meta_examples = " << num_necessary_meta_examples << " delta_num_necessary_meta_examples = "<< delta_num_necessary_meta_examples << endl;

    int current_delta = init_num_missing_bits - module->num_missing_bits;
    double delta_ratio;
    if(delta_num_necessary_meta_examples == 0)
    {
        delta_ratio = 1000000 + current_delta;
    }
    else
    {
        delta_ratio = (double) current_delta / (double) delta_num_necessary_meta_examples;
    }

//    double ret = (double) total_delta_num_bits / (double) num_necessary_meta_examples;

//    use this if want to maximize num solved bits.
//    return current_delta;

    delta_ratio = current_delta;

    return HeuristicScore(
            __builtin_popcount(module->subdomain_mask),
            delta_ratio);
}

static int maximal_factoring_schema_depth_counter = 0;
static int minimal_factoring_schema_depth_counter = 0;

static string best_subdomains[1000] = {};

void MinimalFactoringSchema::calc_masks(int set_init_mask_size)
{
    if(masks.size() >= 1)
    {
        return;
    }
    if(true) {
        vector<vector<int> > masks_by_size = vector<vector<int> >(function_size + 1, vector<int>());

        assert(function_size <= 23);

        int num_masks = (1 << function_size);

        assert(num_masks >= 0);

        for (int i = 1; i <num_masks; i++) {
            if (minimal_factoring_schema_depth_counter < maximal_factoring_schema_depth_counter) {
                i = string_to_bitvector(best_subdomains[minimal_factoring_schema_depth_counter++]);
                masks_by_size[__builtin_popcount(i)].push_back(i);
                break;
            } else {
                masks_by_size[__builtin_popcount(i)].push_back(i);
            }
        }

        for (int i = 0; i < masks_by_size.size(); i++) {
            vector<int> reduced;
            if (i >= set_init_mask_size) {
                for (int j = 0; j < masks_by_size[i].size(); j++) {
                    reduced.push_back(masks_by_size[i][j]);
                    masks.push_back(masks_by_size[i][j]);
                }
            }
            masks_by_size[i] = reduced;
        }
    }
    else {
        string custom_masks[100] =
                {
                        "0000010000100001",
                        "0000110000100001",
                        "0000110000100011",
                        "0000110000100010",
                        "0000110001100001",
                        "0000110001000001",
                        "0000110001100011",
                        "0000110001000011",
                        "0000110001100010",
                        "0000110001000010",
                        "0001110001100011",
                        "0001110001100010",
                        "0001110001000011",
                        "0001110001000010",
                        "0001110001100111",
                        "0001110011100011",
                        "0001110011100111",
                        "0001110011000110",
                        "0011110011100111",
                        "0011110011101111",
                        "0011110111100111",
                        "0011110111101111",
                        "0111110111101111",
                        "0111110111111111",
                        "0111111111101111",
                        "0111111111111111",
                        "1111111111111111",
                        "",
                };
        int i = 0;
        while(custom_masks[i] != "")
        {
            masks.push_back(string_to_bitvector(custom_masks[i]));
            i++;
        }
    }

}

static ofstream fout;

static int set_init_mask_size = 1;
static int skip_after_mask_size_if_not_necessary = 1;
//static string fout_name = "fast_subdomain_masks_3_4_5_6_7_with_repeats_traced_corrected_only_active_eval_delta_heuristic.out";
//static string fout_name = "using_UnionOfPartialFunctions__using_delta_heuristic__subdomain_size_3_4_if_necessary_5_6_7.out";
static string fout_name = "max_pure_delta.out";

MinimalFactoringSchema::MinimalFactoringSchema(vector<MetaExample> _meta_examples, string ordering_name, bool skip) {
    parent_pointer = nullptr;

    assert(!fout.is_open());

    fout.open(ordering_name + "__" + fout_name);

    if (!skip) {
        minimal_factoring_schema_depth_counter = 0;

        ifstream fin("language(n=4,id=10)[rec=0]__third__restricted_partition_size_10__using_UnionOfPartialFunctions__using_delta_heuristic__subdomain_size_1_2_3_if_necessary_4plus.out");

        string line;
        int line_id = 0;
        cout << "Already processed: " << endl;
        bool read = false;
        while (getline(fin, line)) {
            stringstream ss;
            ss << line;
            string first;
            if(!read)
            {
                ss >> first;
                if(first == "missing_bits")
                {
                    read = true;
                }
            }
            else {
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
                    if(third == "0")
                    {
                        break;
                    }
                }
            }
            line_id++;
        }
    }

    fout << "meta_examples " << _meta_examples.size() << endl;
    for(int i = 0;i<_meta_examples.size();i++)
    {
        fout << "\t"+_meta_examples[i].to_string() << endl << std::flush;
    }

    main__minimal_factoring_schema(_meta_examples);

    fout.close();
}

MinimalFactoringSchema::MinimalFactoringSchema(
        vector<MetaExample> _meta_examples, MinimalFactoringSchema *_parent_pointer)
{
    parent_pointer = _parent_pointer;
    main__minimal_factoring_schema(_meta_examples);
}


MinimalFactoringSchema::MinimalFactoringSchema(vector<MetaExample> _meta_examples, string ordering_name, vector<int> _masks)
{
    parent_pointer = nullptr;
    masks = _masks;
    fout.open(ordering_name + "__" + fout_name);
    fout << "meta_examples " << _meta_examples.size() << endl;
    for(int i = 0;i<_meta_examples.size();i++)
    {
        fout << "\t"+_meta_examples[i].to_string() << endl << std::flush;
    }
    main__minimal_factoring_schema(_meta_examples);
    fout.close();
}

void MinimalFactoringSchema::calc_module(int subdomain_mask, Module *module)
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
            get_meta_examples_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
                    subdomain_mask, meta_examples);

//    cout << "num_individually_consistent_for_rest = " << individually_consistent_meta_examples.size() << endl;

    module->compact_poset =
            new CompactPoset(function_size, subdomain_mask, subdomain_mask, individually_consistent_meta_examples);

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
//                    function_size, subdomain_mask, subdomain_mask,
//                    module->module_meta_examples);

//    test_compact_poset_for_consistency_with_all_meta_examples(subdomain_mask, module->compact_poset);

//    cout << "tested mask = " << bitvector_to_str(subdomain_mask, function_size) << " time: " << (double) time(nullptr) - local_time << endl;

    module->meta_examples_after_query =
            get_meta_examples_after_query(subdomain_mask, module->compact_poset, meta_examples, false, false, false);

    module->intermediate_num_missing_bits = get_num_missing_bits(module->meta_examples_after_query);
    int intermediate_delta_num_bits = init_num_missing_bits - module->intermediate_num_missing_bits;
//    cout << "intermediate_num_missing_bits = " << module->intermediate_num_missing_bits << " intermediate_delta_num_bits = " << intermediate_delta_num_bits << endl;

    repeat_apply_parents(module);

    module->num_missing_bits = get_num_missing_bits(module->meta_examples_after_query);
    int delta_num_bits = init_num_missing_bits - module->num_missing_bits;
//    cout << "after_repeat_num_missing_bits = " << module->num_missing_bits << " after_repeat_delta_num_missing_bits = " << delta_num_bits << endl;

}

void MinimalFactoringSchema::main__minimal_factoring_schema(vector<MetaExample> _meta_examples) {

    meta_examples = _meta_examples;
    calc_function_size();

    local_time = time(nullptr);

    if (parent_pointer == nullptr) {
        init_time = local_time;
        root_pointer = this;
        moule_id = 0;
    } else {
        init_time = parent_pointer->init_time;
        root_pointer = parent_pointer->root_pointer;
        moule_id = parent_pointer->moule_id+1;
    }

    calc_masks(set_init_mask_size);

    init_num_missing_bits = get_num_missing_bits(meta_examples);

    if(init_num_missing_bits >= 1) {

        vector<pair<HeuristicScore, int> > mask_ids_by_heuristic;

        bool possible_candidate_found = false;
        int possible_candindate_num_input_bits = -1;
        bool try_all = false;

        for (int mask_id = 0; mask_id < masks.size(); mask_id++) {

            if(possible_candidate_found){
                if (__builtin_popcount(masks[mask_id]) > possible_candindate_num_input_bits+1 || possible_candindate_num_input_bits==4)
                {
                    continue;
                }
            }
//            if (possible_candidate_found) {
//                if (__builtin_popcount(masks[mask_id]) > skip_after_mask_size_if_not_necessary) {
//                    if (!try_all) {
//                        continue;
//                    }
//                }
//            } else {
//                if (__builtin_popcount(masks[mask_id]) > skip_after_mask_size_if_not_necessary) {
////                try_all = true;
//                }
//            }

            Module local_module = Module(this);

//            cout << "working on mask = " << bitvector_to_str(masks[mask_id], function_size) << " time: "
//                 << (double) time(nullptr) - local_time << endl;

            calc_module(masks[mask_id], &local_module);

            local_module.compact_poset->clear();

            HeuristicScore heuristic = calculate_heuristic(&local_module);

            if (heuristic.defined && !possible_candidate_found) {
                possible_candidate_found = true;
                possible_candindate_num_input_bits = heuristic.num_input_bits;
            }

            mask_ids_by_heuristic.push_back(make_pair(heuristic, mask_id));

//            cout << "heuristic = " << fixed << setprecision(4) << heuristic.to_string() << endl;
        }

        sort(mask_ids_by_heuristic.begin(), mask_ids_by_heuristic.end());

        cout << "PRINT mask_ids_by_heuristic" << endl;
        for (int local_id = 0; local_id < mask_ids_by_heuristic.size(); local_id++) {
            int mask_id = mask_ids_by_heuristic[local_id].second;
            cout << bitvector_to_str(masks[mask_id], function_size) << fixed << setprecision(6)
                 << " heuristic_measure = " << mask_ids_by_heuristic[local_id].first.to_string() << endl;
            if(!mask_ids_by_heuristic[local_id].first.defined)
            {
                cout << "..." << endl;
                break;
            }
        }

        assert(mask_ids_by_heuristic.size() >= 1);

        HeuristicScore max_num_consistent_meta_examples = mask_ids_by_heuristic[0].first;

        if (max_num_consistent_meta_examples.defined) {

            if (parent_pointer == nullptr) {
                fout << "missing_bits " << init_num_missing_bits << endl << std::flush;
            }

            best_module = Module(this);

            int best_mask_id = mask_ids_by_heuristic[0].second;

            calc_module(masks[best_mask_id], (&best_module));

            calculate_heuristic((&best_module));

            fout << best_module.print_module_sketch(time(nullptr) - init_time) << std::flush;

            next = new MinimalFactoringSchema(best_module.meta_examples_after_query, this);

            if(parent_pointer == nullptr)
            {

                MinimalFactoringSchema* last = this;
                while(last->next != nullptr)
                {
                    last = last->next;
                }

                fout << last->parent_pointer->best_module.covered_to_string(root_pointer->meta_examples) << endl;

                vector<pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > > all_traces;
                for(int i = 0;i<meta_examples.size();i++) {
                    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace =
                            record_trace_of_query(
                                    meta_examples[i].partial_function);
                    assert(trace.second.first.size() + 1 == trace.second.second.size());
                    all_traces.push_back(trace);
                }
                for(int i = 0;i<all_traces.size();i++)
                {
                    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
                    assert(trace.second.first.size()+1 == trace.second.second.size());
                    fout << "all    ";
                    for(int j = 0;j<trace.first.size(); j++)
                    {
                        fout << trace.first[j].to_string() <<" ";
                    }
                    fout << endl;
                }
                fout << endl;
                for(int i = 0;i<all_traces.size();i++)
                {
                    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
                    assert(trace.second.first.size()+1 == trace.second.second.size());
                    fout << "active ";
                    for(int j = 0;j<trace.second.second.size(); j++)
                    {
                        fout << trace.second.second[j].to_string() << " " ;
                    }
                    fout << endl;
                }
                fout << endl;
                for(int i = 0;i<all_traces.size();i++)
                {
                    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
                    assert(trace.second.first.size()+1 == trace.second.second.size());
                    fout << "masks  ";
                    for(int j = 0;j<trace.second.first.size(); j++)
                    {
                        fout << std::to_string(trace.second.first[j]) << " " ;
                    }
                    fout << endl;
                }
                fout << endl;
                for(int i = 0;i<all_traces.size();i++)
                {
                    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
                    assert(trace.second.first.size()+1 == trace.second.second.size());
                    fout << "all    ";
                    for(int j = 0;j<trace.first.size(); j++)
                    {
                        fout << trace.first[j].to_string() <<" ";
                    }
                    fout << endl;
                    fout << "active ";
                    for(int j = 0;j<trace.second.second.size(); j++)
                    {
                        fout << trace.second.second[j].to_string() << " " ;
                    }
                    fout << endl;
                    fout << "masks  ";
                    for(int j = 0;j<trace.second.first.size(); j++)
                    {
                        fout << std::to_string(trace.second.first[j]) << " " ;
                    }
                    fout << endl;
                }
                fout << endl;
                get_necessary_meta_examples(true);
            }
        } else
        {
            assert(false);
        }
    }
}

vector<MetaExample> MinimalFactoringSchema::get_necessary_meta_examples(bool print) {
    if(next != nullptr)
    {
        return next->get_necessary_meta_examples(print);
    }
    else
    {
        vector<MetaExample> ret_meta_example;
        if(print)
            fout << "necessary_meta_examples " << parent_pointer->best_module.necessary_meta_example_ids.size() << endl;
        for(int i = 0;i<parent_pointer->best_module.necessary_meta_example_ids.size();i++)
        {
            int id = parent_pointer->best_module.necessary_meta_example_ids[i];
                ret_meta_example.push_back(root_pointer->meta_examples[id]);
            ret_meta_example[ret_meta_example.size()-1].idx = i;
            if(print)
                fout << "\t" << root_pointer->meta_examples[id].to_string() << endl;
        }
//        cout << "ret.size() = " << ret_meta_example.size() << endl;
        return ret_meta_example;
    }
}

PartialFunction MinimalFactoringSchema::query(PartialFunction partial_function)
{
    return query(partial_function, nullptr);
}

PartialFunction MinimalFactoringSchema::query(PartialFunction partial_function, Module *pointer_to_stop) {
    vector<PartialFunction> ret;
    vector<int> ret_operators;
    vector<PartialFunction> active_trace;
    ret.push_back(partial_function);
    active_trace.push_back(partial_function);
    record_trace_of_query(partial_function, pointer_to_stop, ret, ret_operators, active_trace);
    return ret[ret.size()-1];
}

pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > >
        MinimalFactoringSchema::record_trace_of_query(PartialFunction partial_function) {
    vector<PartialFunction> ret;
    vector<int> ret_operators;
    vector<PartialFunction> active_trace;
    ret.push_back(partial_function);
    active_trace.push_back(partial_function);
    record_trace_of_query(partial_function, nullptr, ret, ret_operators, active_trace);
    return make_pair(ret, make_pair(ret_operators, active_trace));
}

void MinimalFactoringSchema::record_trace_of_query(
        PartialFunction partial_function, Module *pointer_to_stop,
        vector<PartialFunction> &trace, vector<int> &active_operators,
        vector<PartialFunction> &active_trace)
{
    if(best_module.compact_poset != nullptr && ((&best_module) != pointer_to_stop)) {
        vector<PartialFunction> intermediate_results =
                best_module.compact_poset->query(partial_function);

        PartialFunction intermediate_result = intermediate_results[0];
        for (int k = 1; k < intermediate_results.size(); k++) {
            intermediate_result.apply_common_partition(intermediate_results[k]);
        }

        if(intermediate_result.partition != trace[trace.size()-1].partition)
        {
            active_operators.push_back(moule_id);
            active_trace.push_back(intermediate_result);
        }
        trace.push_back(intermediate_result);
        for(int i = 0;i<best_module.repeats_module_pointers.size();i++)
        {
            vector<PartialFunction> local_intermediate_results =
                    best_module.repeats_module_pointers[i]->compact_poset->query(intermediate_result);
            PartialFunction local_intermediate_result = local_intermediate_results[0];
            for (int k = 1; k < local_intermediate_results.size(); k++) {
                local_intermediate_result.apply_common_partition(local_intermediate_results[k]);
            }
            if(intermediate_result.partition != local_intermediate_result.partition)
            {
                assert((intermediate_result.partition & local_intermediate_result.partition) == intermediate_result.partition);
                active_operators.push_back(best_module.repeats_module_ids[i]);
                active_trace.push_back(local_intermediate_result);
            }
            intermediate_result = local_intermediate_result;
            trace.push_back(intermediate_result);
        }

        if (next == nullptr || intermediate_result.full())
        {
        } else {
            next->record_trace_of_query(intermediate_result, pointer_to_stop, trace, active_operators, active_trace);
        }
    }
    else
    {
        assert(init_num_missing_bits == 0 || (&best_module) == pointer_to_stop);
    }
}

void MinimalFactoringSchema::calc_function_size() {
    function_size = 0;
    if(meta_examples.size() >= 1)
    {
        function_size = meta_examples[0].get_function_size();
        for(int i = 1;i<meta_examples.size();i++)
        {
            assert(meta_examples[i].get_function_size() == function_size);
        }
    }
}


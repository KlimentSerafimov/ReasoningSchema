//
// Created by Kliment Serafimov on 2019-12-18.
//

#include "ReasoningSchemaOptimizer.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "BittreeTaskType.h"
#include "PartialFunction.h"

bool ReasoningSchemaOptimizer::test_compact_poset_for_consistency_with_all_meta_examples(
        MaskAndCost subdomain_mask, CompactPoset *compact_poset) {

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

void ReasoningSchemaOptimizer::repeat_apply_parents(Module *module) {

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

    vector<ReasoningSchemaOptimizer*> parents;
    vector<CompactPoset*> compact_posets;
    vector<Bitvector> best_subdomain_masks;

    ReasoningSchemaOptimizer* at_parent = this;

    while(at_parent != nullptr)
    {
        parents.push_back(at_parent);
        if(at_parent == this)
        {
            compact_posets.push_back(module->compact_poset);
            best_subdomain_masks.push_back(*module->subdomain_mask);

        } else
        {
            compact_posets.push_back(at_parent->best_module.compact_poset);
            best_subdomain_masks.push_back(*at_parent->best_module.subdomain_mask);
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
                    best_subdomain_masks[i], compact_posets[i], active_meta_examples, false, true);

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

HeuristicScore ReasoningSchemaOptimizer::calculate_heuristic(Module* module) {

    if (parent_pointer == nullptr) {
        module->count_occurences =
                vector<vector<pair<int, pair<int, int> > > >
                        (meta_examples.size(), vector<pair<int, pair<int, int> > >());
        module->covering = vector<vector<vector<int> > >();
    } else {
        module->count_occurences = parent_pointer->best_module.count_occurences;
        module->covering = vector<vector<vector<int> > >(
                parent_pointer->best_module.covering.size(), vector<vector<int> >());

    }
    int module_id = (int) module->covering.size();
    module->covering.push_back(vector<vector<int> >(module->equivalent_ids.size(), vector<int>()));

    for (int i = 0; i < module->equivalent_ids.size(); i++) {
        for (int j = 0; j < module->equivalent_ids[i].size(); j++) {
            module->count_occurences[module->equivalent_ids[i][j]].push_back(
                    make_pair(module_id, make_pair(i, j)));
        }
    }

    ReasoningSchemaOptimizer *at_parent = this;
    at_parent = at_parent->parent_pointer;

    while (at_parent != nullptr) {
        module_id--;
        module->covering[module_id] = vector<vector<int> >(
                at_parent->best_module.equivalent_ids.size(), vector<int>());
        at_parent = at_parent->parent_pointer;
    }

    vector<pair<int, int> > meta_examples_id_by_occurences;

    for (int i = 0; i < module->count_occurences.size(); i++) {
        meta_examples_id_by_occurences.push_back(make_pair(module->count_occurences[i].size(), i));
    }

    sort(meta_examples_id_by_occurences.begin(), meta_examples_id_by_occurences.end());
    reverse(meta_examples_id_by_occurences.begin(), meta_examples_id_by_occurences.end());

    for (int i = 0; i < meta_examples_id_by_occurences.size(); i++) {
        int id = meta_examples_id_by_occurences[i].second;
//        cout << "id = " << id << endl;
        bool necessary = false;
        for (int j = 0; j < module->count_occurences[id].size(); j++) {
            int parent_id = module->count_occurences[id][j].first;
            int sub_meta_example_id = module->count_occurences[id][j].second.first;
//            cout << parent_id <<" "<< sub_meta_example_id << " | ";
            if (module->covering[parent_id][sub_meta_example_id].size() == 0) {
                necessary = true;
                break;
            }
        }
//        cout << endl;

        if (necessary) {
            module->necessary_meta_example_ids.push_back(id);

            for (int j = 0; j < module->count_occurences[id].size(); j++) {
                int parent_id = module->count_occurences[id][j].first;
                int sub_meta_example_id = module->count_occurences[id][j].second.first;
                module->covering[parent_id][sub_meta_example_id].push_back(id);
            }
        }
    }

    int num_necessary_meta_examples = (int) module->necessary_meta_example_ids.size();

//    cout << "HERE: init_num_missing_bits" << init_num_missing_bits <<" module->num_missing_bits = "<< module->num_missing_bits << endl;

    if (num_necessary_meta_examples == 0) {
        return HeuristicScore();
    } else {
        assert(num_necessary_meta_examples >= 1);
    }

    if (module->num_missing_bits == init_num_missing_bits) {
        return HeuristicScore();
    } else {
        assert(module->num_missing_bits < init_num_missing_bits);
    }

    int prev_necessary_meta_examples_delta = 0;
    if (parent_pointer != nullptr) {
        prev_necessary_meta_examples_delta = (int) parent_pointer->best_module.necessary_meta_example_ids.size();
    }

    int delta_num_necessary_meta_examples = num_necessary_meta_examples - prev_necessary_meta_examples_delta;

//    cout << "num_necessary_meta_examples = " << num_necessary_meta_examples << " delta_num_necessary_meta_examples = "<< delta_num_necessary_meta_examples << endl;

    int current_delta = init_num_missing_bits - module->num_missing_bits;

    double delta_ratio;
    if (delta_num_necessary_meta_examples == 0) {
        delta_ratio = 1000000 + current_delta;
    } else {
        delta_ratio = (double) current_delta / (double) delta_num_necessary_meta_examples;
    }

//    double ret = (double) total_delta_num_bits / (double) num_necessary_meta_examples;

//    use this if want to maximize num solved bits.
//    return current_delta;

//    delta_ratio = current_delta;

    bool minimize_num_necessary_meta_examples;

    if (metric == min_imp_set) {
        return HeuristicScore(
                module->subdomain_mask->count(),
                delta_ratio);
    } else {
        return HeuristicScore(module->subdomain_mask->count(), current_delta);
    }
}

static int maximal_factoring_schema_depth_counter = 0;
static int minimal_factoring_schema_depth_counter = 0;

static string best_subdomains[1000] = {};

void ReasoningSchemaOptimizer::calc_masks(int set_init_mask_size, int set_end_mask_size)
{
    if(masks.size() >= 1)
    {

    }
    else if(parent_pointer != nullptr)
    {
        bool copy_masks_from_parent = false;
        if(copy_masks_from_parent)
        {
            assert(parent_pointer->masks.size() >= 1);
            masks = parent_pointer->masks;
        }
        else
        {
            //place all_best_from_prev;

            vector<MaskAndCostAndInstantiatedModules*> new_first_bucket;
            set<pair<int, int> > ids_for_new_first_bucket;
            for(int i = 0;i<parent_pointer->mask_ids_by_heuristic.size();i++)
            {
                pair<int, int> id = parent_pointer->mask_ids_by_heuristic[i].get_id();
                int bucket_id = id.first;
                int mask_id = id.second;
                ids_for_new_first_bucket.insert(id);
                assert(bucket_id<parent_pointer->masks.size() && mask_id < parent_pointer->masks[bucket_id].size());
                new_first_bucket.push_back(parent_pointer->masks[bucket_id][mask_id]);
            }

            assert(masks.size() == 0);

            masks.push_back(new_first_bucket);
            for(int i = 0;i<parent_pointer->masks.size();i++)
            {
                if(parent_pointer->masks[i].size()>=1) {
                    masks.emplace_back();
                    for (int j = 0; j < parent_pointer->masks[i].size(); j++) {
                        if (ids_for_new_first_bucket.find(make_pair(i, j)) == ids_for_new_first_bucket.end()) {
                            masks[masks.size() - 1].push_back(parent_pointer->masks[i][j]);
                        }
                    }
                }
            }
        }



    }
    else if(true) {
        vector<vector<int> > masks_by_size = vector<vector<int> >(function_size + 1, vector<int>());

        assert(function_size <= 23);

        int num_masks = (1 << function_size);

        assert(num_masks >= 0);

        for (int i = 1; i < num_masks-1; i++) {
            if (minimal_factoring_schema_depth_counter < maximal_factoring_schema_depth_counter) {
                i = string_to_bitvector(best_subdomains[minimal_factoring_schema_depth_counter++]);
                masks_by_size[__builtin_popcount(i)].push_back(i);
                break;
            } else {
                masks_by_size[__builtin_popcount(i)].push_back(i);
            }
        }

        vector<MaskAndCostAndInstantiatedModules*> tmp_masks;
        for (int i = 0; i < masks_by_size.size(); i++) {
            vector<int> reduced;
            if (i >= set_init_mask_size && i <= set_end_mask_size) {
                for (int j = 0; j < masks_by_size[i].size(); j++) {
                    reduced.push_back(masks_by_size[i][j]);
                    tmp_masks.push_back(new MaskAndCostAndInstantiatedModules(
                            new MaskAndCost((float) i, Bitvector(masks_by_size[i][j], function_size))));
                }
            }
            masks_by_size[i] = reduced;
            masks.push_back(tmp_masks);
        }
    }
    else {
        assert(false);
    }
}

static ofstream fout;

static int set_init_mask_size = 1;
static int skip_after_mask_size_if_not_necessary = 1;
static int set_end_mask_size = 4;
//static string fout_name = "fast_subdomain_masks_3_4_5_6_7_with_repeats_traced_corrected_only_active_eval_delta_heuristic.out";
//static string fout_name = "using_UnionOfPartialFunctions__using_delta_heuristic__subdomain_size_3_4_if_necessary_5_6_7.out";
static string fout_name = "max_progress__sped_up.out";

ReasoningSchemaOptimizer::ReasoningSchemaOptimizer(vector<MetaExample> _meta_examples, string ordering_name) {
    parent_pointer = nullptr;

//    assert(!fout.is_open());
//
//    fout.open(fout_name + " " + ordering_name );
//
//    fout << "meta_examples " << _meta_examples.size() << endl;
//    for(int i = 0;i<_meta_examples.size();i++)
//    {
//        fout << "\t"+_meta_examples[i].to_string() << endl << std::flush;
//        cout<< "print new meta_examples rep" << endl;
//
//        fout << "\t"+meta_example_to_string__one_line(_meta_examples[i]) << endl << std::flush;
////        assert(false);
//
//    }

    main__minimal_factoring_schema(_meta_examples);

    fout.close();
}

ReasoningSchemaOptimizer::ReasoningSchemaOptimizer(
        vector<MetaExample> _meta_examples, ReasoningSchemaOptimizer *_parent_pointer)
{
    parent_pointer = _parent_pointer;
    main__minimal_factoring_schema(_meta_examples);
}


ReasoningSchemaOptimizer::ReasoningSchemaOptimizer(vector<MetaExample> _meta_examples, string ordering_name,
                                                   Prior _masks,
                                                   string dir_path, MetricType metric_type)
{
    metric = metric_type;
    parent_pointer = nullptr;
    masks = _masks;
    fout.open(  dir_path + "/" + ordering_name);
    assert(fout.is_open());
    main__minimal_factoring_schema(_meta_examples);
    fout.close();
}

void ReasoningSchemaOptimizer::calc_module(MaskAndCostAndInstantiatedModules *subdomain_mask, Module *module)
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
                    *subdomain_mask, meta_examples);

//    cout << "num_individually_consistent_for_rest = " << individually_consistent_meta_examples.size() << endl;

    module->compact_poset =
            new CompactPoset(
                    function_size, *subdomain_mask, *subdomain_mask, individually_consistent_meta_examples);

//    cout << "num_individually_consistent_together_in_compact_poset = " << individually_consistent_meta_examples.size() << endl;

    bool true_meta_learning = true;
    if(true_meta_learning) {
        prune_globally_inconsistent_meta_examples(meta_examples, *subdomain_mask, module->compact_poset);
        module->compact_poset->soft_delete_redundant_edges();
    }

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
            get_meta_examples_after_query(*subdomain_mask, module->compact_poset, meta_examples, false, false);

    module->intermediate_num_missing_bits = get_num_missing_bits(module->meta_examples_after_query);
//    int intermediate_delta_num_bits = init_num_missing_bits - module->intermediate_num_missing_bits;
//    cout << "intermediate_num_missing_bits = " << module->intermediate_num_missing_bits << " intermediate_delta_num_bits = " << intermediate_delta_num_bits << endl;

    repeat_apply_parents(module);

    module->num_missing_bits = get_num_missing_bits(module->meta_examples_after_query);
//    int delta_num_bits = init_num_missing_bits - module->num_missing_bits;
//    cout << "after_repeat_num_missing_bits = " << module->num_missing_bits << " after_repeat_delta_num_missing_bits = " << delta_num_bits << endl;

}


bool ReasoningSchemaOptimizer::skip_mask(MaskAndCost subdomain_mask)
{
    if(parent_pointer == nullptr)
    {
        return false;
    }
    else
    {
        if((subdomain_mask & *parent_pointer->best_module.subdomain_mask) != 0)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}

void ReasoningSchemaOptimizer::main__minimal_factoring_schema(vector<MetaExample> _meta_examples) {

    meta_examples = _meta_examples;
    calc_function_size();

    local_time = time(nullptr);

    if (parent_pointer == nullptr) {

        fout << "meta_examples " << meta_examples.size() << endl;
        for(int i = 0;i<meta_examples.size();i++)
        {
//        fout << "\t"+_meta_examples[i].to_string() << endl << std::flush;
            fout << "\t"+meta_example_to_string__one_line(meta_examples[i]) << endl << std::flush;
        }
        init_time = local_time;
        root_pointer = this;
        module_id = 0;
    } else {
        init_time = parent_pointer->init_time;
        root_pointer = parent_pointer->root_pointer;
        module_id = parent_pointer->module_id + 1;
        metric = root_pointer->metric;
    }

    calc_masks(set_init_mask_size, set_end_mask_size);

    init_num_missing_bits = get_num_missing_bits(meta_examples);

    int num_candidates_to_find = 1;

    if(parent_pointer == nullptr)
    {
        assert(init_num_missing_bits >= 1);
    }

    if(init_num_missing_bits >= 1) {

        bool possible_candidate_found = false;

        for(int bucket_id = 0; bucket_id < masks.size(); bucket_id++) {
            heuristic_score_by_bucket_id_by_mask_id.push_back(vector<HeuristicScore>());
            for (int mask_id = 0; mask_id < masks[bucket_id].size(); mask_id++) {

                HeuristicScore heuristic;

                cout << "working on mask = " << bitvector_to_str(*masks[bucket_id][mask_id], function_size) << " time: "
                     << (double) time(nullptr) - local_time << " ";

                if (false && parent_pointer != nullptr && bucket_id < parent_pointer->heuristic_score_by_bucket_id_by_mask_id.size() &&
                mask_id < parent_pointer->heuristic_score_by_bucket_id_by_mask_id[bucket_id].size() &&
                skip_mask(masks[bucket_id][mask_id])) {
                    cout << "skip" << endl;
                    assert(parent_pointer != nullptr);
                    heuristic = parent_pointer->heuristic_score_by_bucket_id_by_mask_id[bucket_id][mask_id];
                    mask_ids_by_heuristic.push_back(make_pair(heuristic, make_pair(bucket_id, mask_id)));
                    heuristic_score_by_bucket_id_by_mask_id[bucket_id].push_back(heuristic);

                } else {
                    cout << "calc" << endl;
                    Module local_module = Module(this);

                    calc_module(masks[bucket_id][mask_id], &local_module);

                    local_module.compact_poset->clear();

                    heuristic = local_module.heuristic_score = calculate_heuristic(&local_module);

                    if(heuristic.defined) {
                        mask_ids_by_heuristic.push_back(make_pair(heuristic, make_pair(bucket_id, mask_id)));
                    }
                    heuristic_score_by_bucket_id_by_mask_id[bucket_id].push_back(heuristic);


                }
                if (heuristic.defined) {
                    assert(heuristic.num_input_bits >= 1);
                    possible_candidate_found = true;
                    num_candidates_to_find--;

                    if(metric == first_from_user_prior)
                        break;
                }

//            cout << "heuristic = " << fixed << setprecision(4) << heuristic.to_string() << endl;
            }
            cout << "bucket_id "<< bucket_id << " num_candidates_remaining " << num_candidates_to_find << " time " << (double) time(nullptr) - local_time << endl;
            if (num_candidates_to_find <= 0) {
                break;
            }
        }

        sort(mask_ids_by_heuristic.begin(), mask_ids_by_heuristic.end());

        cout << "PRINT mask_ids_by_heuristic" << endl;
        for (int local_id = 0; local_id < mask_ids_by_heuristic.size(); local_id++) {
            int mask_id = mask_ids_by_heuristic[local_id].get_id().second;
            int bucket_id = mask_ids_by_heuristic[local_id].get_id().first;

            //assert(masks[bucket_id][mask_id].get_size() == function_size);
            cout << bitvector_to_str(*masks[bucket_id][mask_id], function_size) << fixed << setprecision(6)
                 << " heuristic_measure = " << mask_ids_by_heuristic[local_id].get_score().to_string() << endl;
            if (mask_ids_by_heuristic[local_id].get_score().defined) {
                assert(mask_ids_by_heuristic[local_id].get_score().num_input_bits >= 1);
            } else {
                cout << "..." << endl;
                break;
            }
        }

        assert(mask_ids_by_heuristic.size() >= 1);

        HeuristicScore max_num_consistent_meta_examples = mask_ids_by_heuristic[0].get_score();

        if (max_num_consistent_meta_examples.defined) {

            if (parent_pointer == nullptr) {
                fout << "missing_bits " << init_num_missing_bits << endl << std::flush;
            }

            best_module = Module(this);

            int best_mask_id = mask_ids_by_heuristic[0].get_id().second;
            int best_bucket_id = mask_ids_by_heuristic[0].get_id().first;

            calc_module(masks[best_bucket_id][best_mask_id], (&best_module));

            best_module.heuristic_score = calculate_heuristic((&best_module));

            fout << best_module.print_module_sketch(time(nullptr) - init_time) << std::flush;

            bool local_test = false;
            if(local_test) {
                for (int i = 0; i < meta_examples.size(); i++) {
                    PartialFunction generalization = query(meta_examples[i].partial_function);
                    cout << "query  " << meta_examples[i].to_string() << endl;
                    cout << "result " << generalization.to_string() << endl;
                    cout << endl;
                    if (!meta_examples[i].generalization.is_contained_in(generalization)) {
                        cout << "wrong" << endl;
                        assert(false);
                    } else {
                        assert(meta_examples[i].generalization.is_contained_in(generalization));
                        cout << "ok" << endl;
                    }
                }
            }

            next = new ReasoningSchemaOptimizer(best_module.meta_examples_after_query, this);

            if(parent_pointer == nullptr)
            {
                bool local_test = false;
                if(local_test) {
                    for (int i = 0; i < meta_examples.size(); i++) {
                        PartialFunction generalization = query(meta_examples[i].partial_function);
                        cout << "query  " << meta_examples[i].to_string() << endl;
                        cout << "result " << generalization.to_string() << endl;
                        cout << endl;
                        if (!generalization.is_contained_in(meta_examples[i].generalization)) {
                            cout << "wrong" << endl;
                            assert(false);
                        } else {
                            assert(meta_examples[i].generalization.is_contained_in(generalization));
                            cout << "ok" << endl;
                        }
                    }
                }

                ReasoningSchemaOptimizer* last = this;
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
                    fout << "all    : ";
                    for(int j = 0;j<trace.first.size(); j++)
                    {
                        if(j != 0)
                        {
                            fout << " -> ";
                        }
                        fout << partial_function_to_string__one_line(trace.first[j]);
                    }
                    fout << endl;
                }
                fout << endl;
                for(int i = 0;i<all_traces.size();i++)
                {
                    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
                    assert(trace.second.first.size()+1 == trace.second.second.size());
                    fout << "active : ";
                    for(int j = 0;j<trace.second.second.size(); j++)
                    {
                        if(j != 0)
                        {
                            fout << " -> ";
                        }
                        fout << partial_function_to_string__one_line(trace.second.second[j]);
                    }
                    fout << endl;
                }
                fout << endl;
                for(int i = 0;i<all_traces.size();i++)
                {
                    pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
                    assert(trace.second.first.size()+1 == trace.second.second.size());
                    fout << "masks  : ";
                    for(int j = 0;j<trace.second.first.size(); j++)
                    {
                        fout << std::to_string(trace.second.first[j]) << " " ;
                    }
                    fout << endl;
                }
                fout << endl;
                bool print_verbose = false;
                if(print_verbose) {
                    for (int i = 0; i < all_traces.size(); i++) {
                        pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > > trace = all_traces[i];
                        assert(trace.second.first.size() + 1 == trace.second.second.size());
                        fout << "all    ";
                        for (int j = 0; j < trace.first.size(); j++) {
                            fout << trace.first[j].to_string() << " ";
                        }
                        fout << endl;
                        fout << "active ";
                        for (int j = 0; j < trace.second.second.size(); j++) {
                            fout << trace.second.second[j].to_string() << " ";
                        }
                        fout << endl;
                        fout << "masks  ";
                        for (int j = 0; j < trace.second.first.size(); j++) {
                            fout << std::to_string(trace.second.first[j]) << " ";
                        }
                        fout << endl;
                    }
                    fout << endl;
                }
                get_necessary_meta_examples(true);

            }
        } else
        {
            assert(false);
        }
    }
}

vector<MetaExample> ReasoningSchemaOptimizer::get_necessary_meta_examples(bool print) {
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
            if(print) {
//                fout << "\t" << root_pointer->meta_examples[id].to_string() << endl;

                fout << "\t" << root_pointer->meta_examples[id].partial_function.to_string__one_line() << endl;

//                fout << "\t" << meta_example_to_string__one_line(root_pointer->meta_examples[id]) << endl;
            }
        }
//        cout << "ret.size() = " << ret_meta_example.size() << endl;
        return ret_meta_example;
    }
}

PartialFunction ReasoningSchemaOptimizer::query(PartialFunction partial_function)
{
    return query(partial_function, nullptr);
}

PartialFunction ReasoningSchemaOptimizer::query(PartialFunction partial_function, Module *pointer_to_stop) {
    vector<PartialFunction> ret;
    vector<int> ret_operators;
    vector<PartialFunction> active_trace;
    ret.push_back(partial_function);
    active_trace.push_back(partial_function);
    record_trace_of_query(partial_function, pointer_to_stop, ret, ret_operators, active_trace);
    return ret[ret.size()-1];
}

pair<vector<PartialFunction>, pair<vector<int>, vector<PartialFunction> > >
        ReasoningSchemaOptimizer::record_trace_of_query(PartialFunction partial_function) {
    vector<PartialFunction> ret;
    vector<int> ret_operators;
    vector<PartialFunction> active_trace;
    ret.push_back(partial_function);
    active_trace.push_back(partial_function);
    record_trace_of_query(partial_function, nullptr, ret, ret_operators, active_trace);
    return make_pair(ret, make_pair(ret_operators, active_trace));
}

void ReasoningSchemaOptimizer::record_trace_of_query(
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
            active_operators.push_back(module_id);
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

void ReasoningSchemaOptimizer::calc_function_size() {
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

BittreeTaskTypeAsPartialFunction *ReasoningSchemaOptimizer::get_copy_of_bottree_task_type() {
    assert(meta_examples.size() >= 1);
    BittreeTaskType *to_copy = meta_examples[0].partial_function.bittree_task_type;
    auto *local_bittree_task_type = new BittreeTaskType(nullptr, Name("copy_type"), to_copy, true, true);

    cout << "to_copy: " << to_copy->to_string__one_line(meta_examples[0].partial_function.subtask_depth) << endl;
    cout << "copy   : " << local_bittree_task_type->to_string__one_line(meta_examples[0].partial_function.subtask_depth) << endl;

    auto* partial_function =
            new BittreeTaskTypeAsPartialFunction(local_bittree_task_type, meta_examples[0].partial_function.subtask_depth);

    return partial_function;
}

string ReasoningSchemaOptimizer::bitvector_to_string__one_line(MaskAndCost bitvector) {
    assert(meta_examples.size() >= 1);
    BittreeTaskType *to_copy = meta_examples[0].partial_function.bittree_task_type;
//    auto *local_bittree_task_type = new BittreeTaskType(nullptr, Name("copy_type"), to_copy, true, true);

    string temp = to_copy->to_string__one_line__first_part(meta_examples[0].partial_function.subtask_depth);// << endl;

    string ret = "";

    for(int i = 0, at = 0;i<temp.size();i++)
    {
        if(temp[i] == '0' || temp[i] == '1' || temp[i] == '_')
        {
            ret += std::to_string(bitvector.get_bit(at));
            at++;
        } else{
            assert(temp[i] == ' ');
            ret += temp[i];
        }
    }
    return ret;
}

string ReasoningSchemaOptimizer::partial_function_to_string__one_line(PartialFunction partial_function) {
    assert(meta_examples.size() >= 1);
    BittreeTaskType *local_bittree_task_type = meta_examples[0].partial_function.bittree_task_type;
    string temp = local_bittree_task_type->to_string__one_line__first_part(meta_examples[0].partial_function.subtask_depth);

    string ret = "";

    for(int i = 0, at = 0;i<temp.size();i++)
    {

        if(temp[i] == '0' || temp[i] == '1' || temp[i] == '_')
        {
            ret += partial_function.bit_to_string(at);
            at++;
        } else{
            assert(temp[i] == ' ');
            ret += temp[i];
        }
    }
    return ret;
}

string ReasoningSchemaOptimizer::meta_example_to_string__one_line(MetaExample meta_example)
{
    return partial_function_to_string__one_line(meta_example.partial_function) + " -> " +partial_function_to_string__one_line(meta_example.generalization);
//    return meta_example.partial_function.to_string__one_line()
}

vector<MaskAndCostAndInstantiatedModules*> ReasoningSchemaOptimizer::get_subdomains() {
    ReasoningSchemaOptimizer* at = root_pointer;
    vector<MaskAndCostAndInstantiatedModules*> subdomains;
    while(at!=nullptr)
    {
        if(at->next != nullptr) {
            subdomains.push_back(at->best_module.subdomain_mask);
        }
        at = at->next;
    }
    return subdomains;
}

ReasoningSchemaOptimizer::ReasoningSchemaOptimizer() {

}

vector<Module *> ReasoningSchemaOptimizer::get_modules() {
    vector<Module * > ret;
    ReasoningSchemaOptimizer* at = this;
    while(at != nullptr)
    {
        ret.push_back(&at->best_module);
        at = at->next;
    }
    return ret;
}

vector<Module *> ReasoningSchemaOptimizer::get_program_as_vector_of_modules() {
    vector<Module * > ret;
    ReasoningSchemaOptimizer* at = this;
    while(at != nullptr)
    {
        ret.push_back(&at->best_module);
        for(int i = 0;i<at->best_module.repeats_module_pointers.size(); i++)
        {
            ret.push_back(at->best_module.repeats_module_pointers[i]);
        }
        at = at->next;
    }
    return ret;
}


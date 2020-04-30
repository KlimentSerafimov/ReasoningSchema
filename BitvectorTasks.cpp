//
// Created by Kliment Serafimov on 1/8/20.
//

#include "util.h"
#include "BitvectorTasks.h"
#include "TraceVersionSpace.h"
#include <cstring>
#include <random>
#include <string>

#include <bits/stdc++.h>
#include <sys/stat.h>

#include <iostream>
#include <time.h>

using namespace std;

void BitvectorTasks::populate_ordering_over_boolean_functions_with_small_sum()
{
    assert((function_size-1)%3 == 0);
    int operator_size = function_size/3;
    vector<pair<int, int> > operators;
    vector<pair<pair<int, int>, int> > operators_ids_by_heuristic;
    int first_operand_size = operator_size;
    int second_operand_size = operator_size;
    for(int i = 0;i<(1<<first_operand_size); i++)
    {
        for(int j = 0;j<(1<<second_operand_size); j++)
        {
            operators.push_back(make_pair(i,  j));
            int k = i+j;
            operators_ids_by_heuristic.push_back(make_pair(make_pair(k, max(i, j)), operators.size()-1));
        }
    }
    sort(operators_ids_by_heuristic.begin(), operators_ids_by_heuristic.end());
    for(int i = 0;i<operators_ids_by_heuristic.size();i++)
    {
        int id = operators_ids_by_heuristic[i].second;
        int total_function =
                operators[id].first + (operators[id].second << first_operand_size) + ((operators[id].first + operators[id].second) << (first_operand_size+second_operand_size));
        PartialFunction partial_function = PartialFunction(function_size, total_function);
        ordering_over_boolean_functions.push_back(partial_function);
        cout << partial_function.to_string() << endl;
    }
}


void BitvectorTasks::populate_ordering_over_boolean_functions_with_bitwise_and()
{
    assert(function_size%3 == 0);
    vector<pair<int, int> > operators;
    vector<pair<pair<int, int>, int> > operators_ids_by_heuristic;
    int first_operand_size = 5;
    int second_operand_size = 5;
    for(int i = 0;i<(1<<first_operand_size); i++)
    {
        for(int j = 0;j<(1<<second_operand_size); j++)
        {
            operators.push_back(make_pair(i,  j));
            int k = i&j;
            operators_ids_by_heuristic.push_back(make_pair(make_pair(k, max(i, j)), operators.size()-1));
        }
    }
    sort(operators_ids_by_heuristic.begin(), operators_ids_by_heuristic.end());
    for(int i = 0;i<operators_ids_by_heuristic.size();i++)
    {
        int id = operators_ids_by_heuristic[i].second;
        int total_function =
                operators[id].first + (operators[id].second << first_operand_size) + ((operators[id].first&operators[id].second) << (first_operand_size+second_operand_size));
        PartialFunction partial_function = PartialFunction(function_size, total_function, (1<<16)-1-(1<<15));
        ordering_over_boolean_functions.push_back(partial_function);
        cout << partial_function.to_string() << endl;
    }
}


MetaExample BitvectorTasks::get_meta_example(PartialFunction partial_function) {
    for(int i = 0;i<ordering_over_boolean_functions.size();i++)
    {
        PartialFunction boolean_function = ordering_over_boolean_functions[i];
//        is_generalization_counter += __builtin_popcount(partial_function.partition);
        if(boolean_function.is_contained_in(partial_function))
        {
            return MetaExample(boolean_function.function_size, boolean_function.total_function, partial_function.partition);
        }
    }
    return MetaExample(partial_function.function_size, partial_function.total_function, partial_function.partition, partial_function.partition);
}

BitvectorTasks::BitvectorTasks(int _function_size, int task_id) {
    function_size = _function_size;
    if(task_id == 0)
    {
        populate_ordering_over_boolean_functions_with_small_sum();
    }
    if(task_id == 1)
    {
        populate_ordering_over_boolean_functions_with_bitwise_and();
    }
}

InstanceTree::InstanceTree(BittreeTaskType* _instance, TaskName _task_name) {
    instance = _instance;
    task_name = _task_name;
}

void InstanceTree::prepare_for_deepening(BittreeInputOutputType* delta)
{
    superinstance_type = instance->get_supertask_type(delta);

    vector<BitInBittree*> input_bits;

    memset_visited(vis_bits);
    superinstance_type->io->input->append_bits(input_bits);

    int num_input_bits = 0;
    for(int i = 0;i<input_bits.size();i++)
    {
        if(!input_bits[i]->is_bit_set)
        {
            num_input_bits += 1;
        }
    }

    num_superinstances = (1 << num_input_bits);

    //assert(num_superinstances <= max_instance_tree_degree);

    for(int i = 0; i < num_superinstances; i++)
    {
        superinstances[i] = new BittreeTaskType(
                NULL, Name("superinstances", i), superinstance_type, true);
        if(superinstance_type->decomposition->subtask != NULL)
        {
            superinstances[i]->decomposition = new BittreeTaskDecomposition(
                    superinstances[i], Name("decomposition"), NULL, superinstance_type->decomposition->subtask);
        }

//        cout << "---------------------------------------------------" << endl;
//        cout <<superinstances[i]->to_string() << endl;
        vector<BitInBittree*> local_input_bits;
        memset_visited(vis_bits);
       superinstances[i]->io->input->append_bits(local_input_bits);
        int num_unassigned_bits = 0;
        for(int j = 0, k = 0;j<local_input_bits.size();j++)
        {
            if(local_input_bits[j]->is_bit_set == false)
            {
                num_unassigned_bits++;
                local_input_bits[j]->is_bit_set = true;
                local_input_bits[j]->bit_val = get_bit(i, k);
                k++;
            }
        }
        assert(num_unassigned_bits == num_input_bits);
//        cout <<superinstances[i]->to_string() << endl;
       superinstances[i]->solve(task_name);
//        cout <<superinstances[i]->to_string() << endl;
//        cout << "---------------------------------------------------" << endl;
    }
    prepared_for_deepening = true;
}

void InstanceTree::deepen(BittreeInputOutputType* delta)
{
    if(deepened)
    {
        for (int i = 0; i < num_superinstances; i++) {
            superinstance_trees[i]->deepen(delta);
        }
    }
    else
    {
        if(!prepared_for_deepening)
        {
            prepare_for_deepening(delta);
        }
        assert(superinstance_trees.size() == 0);
        for (int i = 0; i < num_superinstances; i++) {
            superinstance_trees.push_back(
                    new InstanceTree(superinstances[i], task_name));
            superinstance_trees[i]->prepare_for_deepening(delta);
        }
        deepened = true;
    }
}

void InstanceTree::populate_meta_examples(vector<vector<MetaExample> >& ret, int at_depth, int subtask_depth)
{
    if(ret.size() <= at_depth)
    {
        assert(ret.size() == at_depth);
        ret.push_back(vector<MetaExample>());
    }
    if(prepared_for_deepening)
    {
        for(int i = 0;i<num_superinstances;i++)
        {
            ret[at_depth].push_back(
                    superinstances[i]->to_meta_example_of_subtask_decomposition(
                            ret[at_depth].size(), subtask_depth));
        }
    }
    if(deepened)
    {
        assert(superinstance_trees.size() == num_superinstances);
        for(int i = 0; i < superinstance_trees.size();i++)
        {
            superinstance_trees[i]->populate_meta_examples(ret, at_depth + 1, subtask_depth);
        }
    }
}

BittreeTaskInstance::BittreeTaskInstance(BittreeTaskType* _bittree_task_type)
{
    bittree_task_type = _bittree_task_type;

    vector<BitInBittree*> input_bits;
    vector<BitInBittree*> output_bits;

    memset_visited(vis_bits);
    bittree_task_type->io->input->append_bits(input_bits);
    bittree_task_type->io->output->append_bits(output_bits);

    int num_input_bits = input_bits.size();
    int num_instances = (1<<num_input_bits);
    for(int i = 0; i<num_instances; i++)
    {
        instances.push_back(BittreeTaskType(NULL, Name("instances", instances.size()), bittree_task_type, true));
        vector<BitInBittree*> local_input_bits;
        memset_visited(vis_bits);
        instances[i].io->input->append_bits(local_input_bits);
        assert(local_input_bits.size() == num_input_bits);
        for(int j = 0;j<local_input_bits.size();j++)
        {
            local_input_bits[j]->is_bit_set = true;
            local_input_bits[j]->bit_val = get_bit(i, j);
        }
//        cout << instances[i].to_string() << endl;
    }
}

vector<BittreeTaskType *>
BitvectorTasks::get_multi_task_type(BittreeTypeExpression *type_expression, int init_num_iter) {
    int num_iter = init_num_iter;

    vector<BittreeTaskType *> multi_task_type;
    for (int i = 0; i < num_iter; i++) {
        if(i == 0)
        {
            multi_task_type.push_back(type_expression->base_task_type->get_supertask_type(type_expression->init_delta_task_type));
        } else{
            multi_task_type.push_back(multi_task_type[i-1]->get_supertask_type(type_expression->delta_task_type));
        }
        cout << multi_task_type[i]->to_string() << endl;
    }
    return multi_task_type;

}

vector<vector<vector<Bitvector> > >
BitvectorTasks::masks_generator(int num_subtasks, int max_masks_size, int min_mask_size, int num_first_in_prior, vector<BittreeTaskType*> multi_task_type)
{
    if(true)
    {
        vector<vector<vector<Bitvector> > > masks;
        for (int i = 0; i < multi_task_type.size(); i++) {
            masks.push_back(multi_task_type[i]->to_meta_example(-1, num_subtasks).get_masks(min_mask_size, max_masks_size, num_first_in_prior));
        }

        return masks;

    }
    else {
        // need to refactor using multi_task_type instead of curriculum
//        vector<vector<Bitvector> > masks;
//
//        vector<BittreeTaskType *> curriculum;
//        int num_subtree_markers = 1;
//        int max_mask_type_depth = 100;
//        curriculum.push_back(type_expression->base_task_type);
//
////        memset_visited(vis_type, num_subtree_markers);
////        BittreeProgram program = BittreeProgram(curriculum[0], NULL, num_subtree_markers);
////        masks.push_back(program.all_bittree_masks_as_bitvectors);
////        num_iter--;
//        for (int i = 0; i < num_iter; i++) {
//            curriculum.push_back(curriculum[i]->get_supertask_type(type_expression->delta_task_type));
////            cout << curriculum[i + 1]->to_string() << endl;
//            memset_visited(vis_type, num_subtree_markers);
//            BittreeProgram program = BittreeProgram(curriculum[i + 1], NULL, num_subtree_markers, max_mask_type_depth, num_subtasks);
//            masks.push_back(program.all_bittree_masks_as_bitvectors);
//        }
//
////      BittreeTaskInstance instances = BittreeTaskInstance(curriculum[4]);
//
//        vector<vector<vector<Bitvector> > > ret;
//        ret.push_back(masks);
//
//        return ret;
    }

}

vector<vector<MetaExample> >
BitvectorTasks::get_meta_examples(BittreeTypeExpression *type_expression, TaskName task_name, int init_num_iter,
                                  int subtask_depth)
{
    int num_iter = init_num_iter;
    type_expression->base_task_type->solve(task_name);
    num_iter--;

//    cout << type_expression->base_task_type->to_string() << endl;

    InstanceTree instances = InstanceTree(type_expression->base_task_type, task_name);
    instances.prepare_for_deepening(type_expression->init_delta_task_type);
//    instances.deepen(type_expression->init_delta_task_type);
//
//    instances.prepare_for_deepening(type_expression->delta_task_type);
//    num_iter--;

    for(int iter = 0;iter<num_iter;iter++)
    {
        cout << "GENERATING DATA FOR ITER: " << iter << endl;
//        vector<vector<MetaExample> > meta_examples;
//        instances.populate_meta_examples(meta_examples, 0, subtask_depth);
//        for(int i = 0;i<meta_examples.size();i++)
//        {
//            for(int j = 0;j<meta_examples[i].size();j++)
//            {
//                cout << meta_examples[i][j].to_string() << endl;
//            }
//            cout << endl;
//        }
//        cout << "----------------------------------------" << endl;

        instances.deepen(type_expression->delta_task_type);
        cout << "DONE GENERATING DATA FOR ITER: " << iter << endl;
    }

    vector<vector<MetaExample> > meta_examples;
    instances.populate_meta_examples(meta_examples, 0, subtask_depth);

    assert(meta_examples.size() == init_num_iter);

    return meta_examples;
}

vector<Bitvector> get_next_subdomains(
        MetricType metric, string dir_path, string init_language_name,
        vector<Bitvector> subdomains, BittreeTaskType * current_bittree, BittreeTaskType * next_bittree, int num_prev_subtasks)
{
    static ofstream fout;

    string fout_name = "subdomains__metric=" + metric_type_name[metric];
    fout.open(dir_path + "/" + fout_name + init_language_name);
    for (int j = 0; j < subdomains.size(); j++) {
        fout << subdomains[j].to_string() << endl;
    }
//                assert(subdomains.size() == multi_task_type.size());

    fout << endl;

    for (int j = 0; j < subdomains.size(); j++) {
        BittreeTaskTypeAsPartialFunction bittree_as_partial = BittreeTaskTypeAsPartialFunction(
                current_bittree, num_prev_subtasks);
        bittree_as_partial.assign_bits(subdomains[j]);
        fout << bittree_as_partial.to_string__one_line() << endl;
    }

    vector<Bitvector> next_subdomains;
    if (next_bittree != NULL) {

        set<Bitvector> next_subdomain_set;

        for (int subdomain_id = 0; subdomain_id < subdomains.size(); subdomain_id++) {
            BittreeTaskTypeAsPartialFunction bittree_as_partial = BittreeTaskTypeAsPartialFunction(
                    current_bittree, num_prev_subtasks);


            BittreeTaskTypeAsPartialFunction next_bittree_as_partial = BittreeTaskTypeAsPartialFunction(
                    next_bittree, num_prev_subtasks);

//                        fout << "pre_init from: " << endl;
//                        fout << bittree_as_partial.to_string__one_line() << endl;
//                        fout << "pre_init to: " << endl;
//                        fout << next_bittree_as_partial.to_string__one_line() << endl;

            bittree_as_partial.assign_bits(subdomains[subdomain_id]);
            bittree_as_partial.update_bitvector();

//                        fout << "init from: " << endl;
//                        fout << bittree_as_partial.to_string__one_line() << endl;
//
//                        fout << "init to: " << endl;
//                        fout << next_bittree_as_partial.to_string__one_line() << endl;

            for (int bit_id = 0, next_bit_id = 0; next_bit_id < next_bittree_as_partial.bits.size();) {

                if (bit_id < bittree_as_partial.bits.size()) {

                    vector<string> path = bittree_as_partial.get_path_of_bit_id(bit_id);

                    vector<string> next_path = next_bittree_as_partial.get_path_of_bit_id(next_bit_id);

//                                for (int ii = 0; ii < path.size(); ii++) {
//                                    fout << path[ii] << " ";
//                                }
//                                fout << endl;
//                                for (int ii = 0; ii < next_path.size(); ii++) {
//                                    fout << next_path[ii] << " ";
//                                }
//                                fout << endl;

                    if (path == next_path) {
//                                    fout << "the_same" << endl;

                        next_bittree_as_partial.bits[next_bit_id]->is_bit_set = true;
                        next_bittree_as_partial.bits[next_bit_id]->bit_val = bittree_as_partial.bits[bit_id]->bit_val;

                        next_bit_id++;
                        bit_id++;
                    } else {
                        next_bittree_as_partial.bits[next_bit_id]->is_bit_set = true;
                        next_bittree_as_partial.bits[next_bit_id]->bit_val = 0;
//                                    fout << "not the_same" << endl;
                        next_bit_id++;
                    }
                } else {
//                                fout << "outside" << endl;
                    next_bittree_as_partial.bits[next_bit_id]->is_bit_set = true;
                    next_bittree_as_partial.bits[next_bit_id]->bit_val = 0;
                    next_bit_id++;
                }

            }

            next_bittree_as_partial.update_bitvector();

            vector<Bitvector> local_variety = next_bittree_as_partial.generate_variety();

            for (int i = 0; i < local_variety.size(); i++) {
                next_subdomain_set.insert(local_variety[i]);
            }

//                        next_subdomains.push_back(next_bittree_as_partial.total_function);

//                        fout << "from: " << endl;
//                        fout << bittree_as_partial.to_string__one_line() << endl;
//                        fout << "to: " << endl;
////                        fout << next_bittree_as_partial.to_string__one_line() << endl;
            fout << next_bittree_as_partial.to_string__one_line() << endl;
////                        fout << next_bittree_as_partial.total_function.to_string() << endl;
//                        fout << endl;
        }

        for (auto subdomain : next_subdomain_set) {
            next_subdomains.push_back(subdomain);
        }

        for (int subdomain_id = 0; subdomain_id < next_subdomains.size(); subdomain_id++) {
//                        next_subdomains[subdomain_id].set_size(subdomains[subdomain_id].get_size());
            fout << next_subdomains[subdomain_id].to_string() << endl;
        }

    }

    fout.close();
    return next_subdomains;
}


BitvectorTasks::BitvectorTasks(TaskName task_name, int init_iter, int num_iter, int recursive_rep_set_depth,
                               MetricType metric, int min_mask_size, int max_mask_size, int num_prev_subtasks,
                               string dir_path, int num_first_in_prior, int seed_train_set, int num_minimization_steps,
                               double minimization_fraction)
{

    if(false)
    {
        int num_subtasks = 1;
        BittreeTypeExpression type_expression_for_masks = BittreeTypeExpression(task_name);
        vector<BittreeTaskType*> curriculum;
        int num_iter = 4;
        int num_subtree_markers = 1;
        int max_mask_type_depth = 1;
        curriculum.push_back(type_expression_for_masks.base_task_type);

        memset_visited(vis_type, num_subtree_markers);
        BittreeProgram program = BittreeProgram(curriculum[0], NULL, num_subtree_markers, max_mask_type_depth, num_subtasks);
        for (int i = 0; i < num_iter; i++) {
            curriculum.push_back(curriculum[i]->get_supertask_type(type_expression_for_masks.delta_task_type));
            cout << curriculum[i + 1]->to_string() << endl;
            memset_visited(vis_type, num_subtree_markers);
            BittreeProgram program = BittreeProgram(curriculum[i + 1], NULL, num_subtree_markers, max_mask_type_depth, num_subtasks);
        }

//      BittreeTaskInstance instances = BittreeTaskInstance(curriculum[4]);

    }
    else
    {
        BittreeTypeExpression type_expression_for_meta_examples = BittreeTypeExpression(task_name);
        BittreeTypeExpression type_expression_for_multi_task_set = BittreeTypeExpression(task_name);


        if(task_name.num_iter_defined)
        {
            num_iter = task_name.num_iter;
        }

        vector<BittreeTaskType *> multi_task_type = get_multi_task_type(&type_expression_for_multi_task_set, num_iter);

        vector<vector<MetaExample> > meta_examples = get_meta_examples(
                &type_expression_for_meta_examples, task_name, num_iter, num_prev_subtasks);

        dir_path =
                 "task_name=" + task_name.get_task_name() +
                 "-gen=48-init_iter=" + std::to_string(init_iter) +
                 "-end_iter=" + std::to_string(num_iter) +
                 "-num_prev_subtasks=" + std::to_string(num_prev_subtasks) +
                 "-min_mask_size=" +std::to_string(min_mask_size) +
                 "-max_mask_size=" +std::to_string(max_mask_size) +
                 "-get_fst_from_pior="+std::to_string(num_first_in_prior) +
                 "-metric=" + metric_type_name[metric]+
                 "-seed_train_set=" + std::to_string(seed_train_set)+
                 "-num_minimize_steps="+std::to_string(num_minimization_steps)+
                 "-minimize_fraction="+std::to_string(minimization_fraction);

        char _dir_path[dir_path.length()+1];

        int check_mkdir = mkdir( dir_path.c_str(), 0777);
        if (check_mkdir == -1){
            cerr << "Error :  " << strerror(errno) << endl;
            assert(false);
        }
        else {
            cout << "Directory created " << _dir_path << endl;
            cout << "should be " << dir_path << endl;
//            dir_path = "";
//            for (int i = 0; i < dir_path.size(); i++) {
//                dir_path[i] += _dir_path[i];
//            }

        }

        vector<Bitvector> next_subdomains;

        vector<vector<vector<Bitvector> > > masks;

        masks.reserve(init_iter);
        for(int task_id = 0; task_id < init_iter;task_id++)
        {
            masks.emplace_back();
        }

        ofstream summary(dir_path + "/summary");
        ofstream summary_with_times(dir_path + "/summary_with_times");

        time_t init_time = time(nullptr);

        for(int task_id = init_iter; task_id < meta_examples.size(); task_id++) {

            masks.emplace_back();

            assert(task_id == masks.size()-1);

            bool is_first = task_id == init_iter;

            if(next_subdomains.size() != 0)
            {
                cout << "with_alternative:" << endl;

                for(int i = 0; i<next_subdomains.size();i++)
                {
                    cout << next_subdomains[i].to_string() << endl;
                }
                cout << endl;

                bool first_from_prior_from_prev_task = false;
                if(first_from_prior_from_prev_task) {
                    for (int i = 0; i < next_subdomains.size(); i++) {
                        vector<Bitvector> singleton;
                        singleton.push_back(next_subdomains[i]);
                        masks[task_id].push_back(singleton);
                    }
                }
                else
                {
                    masks[task_id].push_back(next_subdomains);
                }

//                masks[task_id].insert(masks[task_id].begin(), next_subdomains);

                next_subdomains.clear();
            }
//            else {
                multi_task_type[task_id]->to_meta_example(-1, num_prev_subtasks).append_to_masks(
                        min_mask_size, max_mask_size, num_first_in_prior, masks[task_id]);
//            }
            string language_name =
                    "[task_id=" + std::to_string(task_id+1) + "]";

            assert(meta_examples[task_id].size() > 0);
            assert(masks[task_id].size() > 0);
            assert(masks[task_id][0].size() > 0);

            int len_domain_of_meta_example =  meta_examples[task_id][0].get_function_size();
            int len_mask = masks[task_id][0][0].get_size();

            cout << meta_examples[task_id][0].partial_function.to_string__one_line() << endl;

            assert(len_domain_of_meta_example == len_mask);

//            vector<Bitvector> masks =
//                    meta_examples[task_id][0].get_masks(max_mask_size);

            for(int j = 0;j<masks[task_id].size(); j++) {
                for (int k = 0; k < masks[task_id][j].size(); k++) {
                    cout << bitvector_to_str(masks[task_id][j][k], masks[task_id][j][k].get_size()) << endl;
                }
                cout << endl;
            }



            //ideas:
            //multi-objective beam search
            //  objectives include num meta examples necessary to program, node degree, num bits modeled, num instances modeled, primitive size
            //  intermediate bits of computation are those that segment the dataset such that single operators can now apply to a wider domain.
            //  this leads to emergent task decomposition
            //  optimize for task decomposition description in various cost semantics; type size, runtime, memory, depth of compilation.

            //the ordering based on the structured mask ordering can be used to discover the type (init and delta)

            //project the ordering of the subdomain masks onto the language of f(n) = ordering over the subdomain masks that solves the problem. 

            bool train_set_minimization = !is_first;
            if(train_set_minimization) {

                vector<MetaExample> test_meta_examples = meta_examples[task_id];
                vector<MetaExample> train_meta_examples;

                std::shuffle(test_meta_examples.begin(), test_meta_examples.end(), std::mt19937(std::random_device()()));

                for(int i = 0;i<test_meta_examples.size();i++)
                {
                    test_meta_examples[i].idx = i;
                }

                for (int i = 0; i < min((int)test_meta_examples.size(), seed_train_set); i ++) {
                    train_meta_examples.push_back(test_meta_examples[i]);
                    train_meta_examples.back().idx = (int)train_meta_examples.size()-1;
                }

                string init_language_name = language_name;

                vector<Bitvector> subdomains;
                int min_train_set_size = test_meta_examples.size();
                vector<MetaExample> min_train_meta_examples;

                summary << "task_id " << task_id+1 << endl;
                summary_with_times << "task_id_" << task_id+1<<" ";

                for(int minimization_step = 0; minimization_step < num_minimization_steps;minimization_step++) {
                    bool all_solved = false;
                    vector<Bitvector> local_subdomains;
                    while (!all_solved) {
                        language_name = init_language_name + "__min_step="+std::to_string(minimization_step) + "__size_of_train_set=" +
                                        std::to_string(train_meta_examples.size());

                        ReasoningSchemaOptimizer my_schema =
                                ReasoningSchemaOptimizer(
                                        train_meta_examples, language_name, masks[task_id], dir_path, metric);

                        all_solved = true;
                        int num_added = 0;
                        for (int i = 0; i < test_meta_examples.size(); i++) {
                            PartialFunction generalization = my_schema.query(test_meta_examples[i].partial_function);
                            cout << "query  " << test_meta_examples[i].to_string() << endl;
                            cout << "result " << generalization.to_string() << endl;
                            cout << endl;
                            if (!generalization.is_contained_in(test_meta_examples[i].generalization)) {
                                cout << "wrong" << endl;
                                train_meta_examples.push_back(test_meta_examples[i]);
                                train_meta_examples.back().idx = train_meta_examples.size() - 1;
                                all_solved = false;
                                num_added += 1;
                                if (num_added >= seed_train_set) {
                                    cout << "break" << endl;
                                    break;
                                }
                            } else{
                                assert(test_meta_examples[i].generalization.is_contained_in(generalization));
                                cout << "ok" << endl;
                            }
                        }

                        if(all_solved)
                        {
                            local_subdomains = my_schema.get_subdomains();
                        }

                        if(train_meta_examples.size() >= min_train_set_size)
                        {
                            break;
                        }
                    }
                    if(all_solved) {
                        summary << train_meta_examples.size() << endl;
                        summary_with_times << train_meta_examples.size() << " " << (time(nullptr) - init_time) << endl;

                        if (min_train_set_size > train_meta_examples.size()) {
                            min_train_set_size = train_meta_examples.size();
                            min_train_meta_examples = train_meta_examples;
                            assert(local_subdomains.size() >= 1);
                            subdomains = local_subdomains;
                        }
                    }
                    train_meta_examples = min_train_meta_examples;

                    std::shuffle(train_meta_examples.begin(), train_meta_examples.end(), std::mt19937(std::random_device()()));

                    vector<MetaExample> new_train_set;

                    new_train_set.reserve(train_meta_examples.size()*minimization_fraction);
                    for(int i = 0;i<train_meta_examples.size()*minimization_fraction;i++)
                    {
                        new_train_set.emplace_back(train_meta_examples[i]);
                        new_train_set[i].idx = i;
                    }
                    train_meta_examples = new_train_set;
                }


                BittreeTaskType* next_bittree_task_type = NULL;
                if(task_id + 1 < multi_task_type.size())
                {
                    next_bittree_task_type = multi_task_type[task_id+1];
                }
                next_subdomains = get_next_subdomains(
                        metric, dir_path, init_language_name, subdomains, multi_task_type[task_id], next_bittree_task_type, num_prev_subtasks);

            } else {

                if (true) {
                    vector<MetaExample> local_meta_examples = meta_examples[task_id];
                    int prev_meta_examples_size = -1;
                    int now_meta_examples_size = (int) local_meta_examples.size();
                    int rec_id = 0;

                    vector<Bitvector> subdomains;

                    string init_language_name = language_name;

                    do {
                        prev_meta_examples_size = now_meta_examples_size;

                        language_name = language_name + "__rec=" + std::to_string(rec_id);

                        ReasoningSchemaOptimizer my_schema =
                                ReasoningSchemaOptimizer(
                                        local_meta_examples, language_name, masks[task_id], dir_path, metric);

                        for (int i = 0; i < local_meta_examples.size(); i++) {
                            PartialFunction generalization = my_schema.query(local_meta_examples[i].partial_function);
                            cout << "query  " << local_meta_examples[i].to_string() << endl;
                            cout << "result " << generalization.to_string() << endl;
                            cout << endl;
                            assert(generalization.is_contained_in(local_meta_examples[i].generalization));
                            assert(local_meta_examples[i].generalization.is_contained_in(generalization));
                        }
                        cout << "TESTING DONE. ALL CORRECT" << endl;

                        local_meta_examples = my_schema.get_necessary_meta_examples(false);

                        now_meta_examples_size = (int) local_meta_examples.size();

                        if (rec_id == 0) {
                            subdomains = my_schema.get_subdomains();
                        }

                        rec_id++;
                        if (recursive_rep_set_depth != -1 && rec_id > recursive_rep_set_depth) {
                            break;
                        }
                    } while (now_meta_examples_size != prev_meta_examples_size);

                    BittreeTaskType* next_bittree_task_type = NULL;
                    if(task_id +1 < multi_task_type.size())
                    {
                        next_bittree_task_type = multi_task_type[task_id+1];
                    }
                    next_subdomains =
                            get_next_subdomains(metric, dir_path, init_language_name, subdomains, multi_task_type[task_id], next_bittree_task_type, num_prev_subtasks);

                } else if (true) {
                    ReasoningSchemaOptimizer my_schema =
                            ReasoningSchemaOptimizer(
                                    meta_examples[task_id], language_name, masks[task_id], dir_path, metric);

                    for (int j = 0; j < meta_examples[task_id].size(); j++) {
                        PartialFunction generalization = my_schema.query(meta_examples[task_id][j].partial_function);
                        cout << "query  " << meta_examples[task_id][j].to_string() << endl;
                        cout << "result " << generalization.to_string() << endl;
                        cout << endl;
                        assert(generalization.is_contained_in(meta_examples[task_id][j].generalization));
                    }
                    cout << "TESTING DONE. ALL CORRECT" << endl;

                    vector<MetaExample> necessary_meta_examples = my_schema.get_necessary_meta_examples(false);
                } else {
                    cout << "Need to send a prior over Bitmasks as a vector of vectors of bitvectors" << endl;
                    assert(false);
                    TraceVersionSpace trace_version_space = TraceVersionSpace(meta_examples[task_id],
                                                                              masks[task_id][0]);
                }
            }
        }
    }
}


class SubdomainScore
{

};

//Goal: implement Solve(Task, StateLatticeGenerator, LatticeCompressor)
//Task is a untyped bitvector mapping
//StateLatticeGenerator: Task -> StateLattice
//StateLattice: TraceVersionSpace: generated from instantiating a composition of PrimitiveTemplates
//LatticeCompressor: SubdomainLattice -> program that when executed generates a trace (where trace: io->input->output path) in StateLattice

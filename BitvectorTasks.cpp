//
// Created by Kliment Serafimov on 1/8/20.
//

#include "util.h"
#include "BitvectorTasks.h"
#include "TraceVersionSpace.h"
#include <cstring>
#include <random>
#include <string>

//#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fstream>

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

InstanceTree::InstanceTree(BittreeTaskType* _instance, Task *_task_name) {
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
                nullptr, Name("superinstances", i), superinstance_type, true);
        if(superinstance_type->decomposition->subtask != nullptr)
        {
            superinstances[i]->decomposition = new BittreeTaskDecomposition(
                    superinstances[i], Name("decomposition"), nullptr, superinstance_type->decomposition->subtask);
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
        assert(superinstance_trees_size == 0);
        for (int i = 0; i < num_superinstances; i++) {
            superinstance_trees[superinstance_trees_size++] =
                    new InstanceTree(superinstances[i], task_name);
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
        assert(superinstance_trees_size == num_superinstances);
        for(int i = 0; i < superinstance_trees_size;i++)
        {
            superinstance_trees[i]->populate_meta_examples(ret, at_depth + 1, subtask_depth);
        }
    }
}

void InstanceTree::vectorize_instance_tree(vector<vector<InstanceTree*> >& ret, int at_depth)
{
    if(ret.size() <= at_depth)
    {
        assert(ret.size() == at_depth);
        ret.push_back(vector<InstanceTree*>());
    }
    if(prepared_for_deepening)
    {
        if(num_superinstances == superinstance_trees_size)
        {
            for (int i = 0; i < num_superinstances; i++) {
                assert(superinstance_trees[i]->instance != nullptr);
            }
        }
        else
        {
            assert(superinstance_trees_size == 0);
            for (int i = 0; i < num_superinstances; i++)
            {
                superinstance_trees[superinstance_trees_size++] =
                        new InstanceTree(superinstances[i], task_name);
            }
        }
        for(int i = 0;i<num_superinstances;i++)
        {
            ret[at_depth].push_back(superinstance_trees[i]);
        }
    }
    if(deepened)
    {
        assert(superinstance_trees_size == num_superinstances);
        for(int i = 0; i < superinstance_trees_size;i++)
        {
            superinstance_trees[i]->vectorize_instance_tree(ret, at_depth + 1);
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
        instances.push_back(BittreeTaskType(nullptr, Name("instances", instances.size()), bittree_task_type, true));
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

vector<vector<vector<MaskAndCost> > >
BitvectorTasks::masks_generator(int num_subtasks, int max_masks_size, int min_mask_size, int num_first_in_prior, vector<BittreeTaskType*> multi_task_type)
{
    if(true)
    {
        vector<vector<vector<MaskAndCost> > > masks;
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
////        BittreeProgram program = BittreeProgram(curriculum[0], nullptr, num_subtree_markers);
////        masks.push_back(program.all_bittree_masks_as_bitvectors);
////        num_iter--;
//        for (int i = 0; i < num_iter; i++) {
//            curriculum.push_back(curriculum[i]->get_supertask_type(type_expression->delta_task_type));
////            cout << curriculum[i + 1]->to_string() << endl;
//            memset_visited(vis_type, num_subtree_markers);
//            BittreeProgram program = BittreeProgram(curriculum[i + 1], nullptr, num_subtree_markers, max_mask_type_depth, num_subtasks);
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

void
BitvectorTasks::get_meta_examples(BittreeTypeExpression *type_expression, Task *task_name, int init_num_iter,
                                  int subtask_depth, vector<vector<MetaExample> >& ret_meta_examples,
                                  vector<vector<InstanceTree*> >& ret_inst_trees)
{
    int num_iter = init_num_iter;
    type_expression->base_task_type->solve(task_name);
    num_iter--;

//    cout << type_expression->base_task_type->to_string() << endl;

    instance_tree = InstanceTree(type_expression->base_task_type, task_name);
    instance_tree.prepare_for_deepening(type_expression->init_delta_task_type);
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

        instance_tree.deepen(type_expression->delta_task_type);
        cout << "DONE GENERATING DATA FOR ITER: " << iter << endl;
    }

    instance_tree.populate_meta_examples(ret_meta_examples, 0, subtask_depth);
    instance_tree.vectorize_instance_tree(ret_inst_trees, 0);

    assert(ret_meta_examples.size() == init_num_iter);
    assert(ret_inst_trees.size() == init_num_iter);

}

vector<MaskAndCost> BitvectorTasks::get_next_subdomains(
        MetricType metric, string dir_path, string init_language_name,
        vector<MaskAndCost> & subdomains, BittreeTaskType * current_bittree, BittreeTaskType * next_bittree, int num_prev_subtasks, int task_id)
{
    ofstream fout;

    string fout_name = "subdomains__metric=" + metric_type_name[metric];
    fout.open(dir_path + "/" + fout_name + init_language_name);
    for (int j = 0; j < subdomains.size(); j++) {
        fout << subdomains[j].to_string() << endl;
    }
//                assert(subdomains.s ize() == multi_task_type.size());

    fout << endl;

    for (int j = 0; j < subdomains.size(); j++) {
        BittreeTaskTypeAsPartialFunction bittree_as_partial = BittreeTaskTypeAsPartialFunction(
                current_bittree, num_prev_subtasks);
        bittree_as_partial.assign_bits(subdomains[j]);
        fout << bittree_as_partial.to_string__one_line() << endl;
    }

    vector<MaskAndCost> next_subdomains;
    if (next_bittree != nullptr)
    {

        set<MaskAndCost> next_subdomain_set;

        for (int subdomain_id = 0; subdomain_id < subdomains.size(); subdomain_id++) {
            BittreeTaskTypeAsPartialFunction* bittree_as_partial = subdomains[subdomain_id].now_canvas;

            vector<BittreeTaskType*> next_current_bittree =
                    get_multi_task_type(new BittreeTypeExpression(task_name), task_id+2);
            BittreeTaskTypeAsPartialFunction* next_bittree_as_partial = new BittreeTaskTypeAsPartialFunction(
                    next_current_bittree[next_current_bittree.size()-1], num_prev_subtasks);

            for (int bit_id = 0, next_bit_id = 0; next_bit_id < next_bittree_as_partial->bits.size();) {

                if (bit_id < bittree_as_partial->bits.size()) {

                    vector<string> path = bittree_as_partial->get_path_of_bit_id(bit_id);

                    vector<string> next_path = next_bittree_as_partial->get_path_of_bit_id(next_bit_id);

                    if (path == next_path) {
                        next_bittree_as_partial->bits[next_bit_id]->is_bit_set = true;
                        next_bittree_as_partial->bits[next_bit_id]->bit_val = bittree_as_partial->bits[bit_id]->bit_val;

                        next_bit_id++;
                        bit_id++;
                    } else {
                        next_bittree_as_partial->bits[next_bit_id]->is_bit_set = true;
                        next_bittree_as_partial->bits[next_bit_id]->bit_val = 0;
                        next_bit_id++;
                    }
                } else {
                    next_bittree_as_partial->bits[next_bit_id]->is_bit_set = true;
                    next_bittree_as_partial->bits[next_bit_id]->bit_val = 0;
                    next_bit_id++;
                }
            }

            next_bittree_as_partial->update_bitvector();

            vector<MaskAndCost> local_variety = next_bittree_as_partial->generate_variety(&fout);

            subdomains[subdomain_id].set_next_bittree_as_partial(next_bittree_as_partial);
            subdomains[subdomain_id].set_local_variety(local_variety);

            for (int i = 0; i < local_variety.size(); i++) {
                next_subdomain_set.insert(local_variety[i]);
            }

            fout << next_bittree_as_partial->to_string__one_line() << endl;
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


void run_bittree_program(Task * task_name)
{

    int num_subtasks = 1;
    BittreeTypeExpression type_expression_for_masks = BittreeTypeExpression(task_name);
    vector<BittreeTaskType*> curriculum;
    int num_iter = 4;
    int num_subtree_markers = 100;
    int max_mask_type_depth = 100;
    curriculum.push_back(type_expression_for_masks.base_task_type);

    memset_visited(vis_type, num_subtree_markers);
    BittreeProgram program = BittreeProgram(curriculum[0], nullptr, num_subtree_markers, max_mask_type_depth, num_subtasks);
    for (int i = 0; i < num_iter; i++) {
        curriculum.push_back(curriculum[i]->get_supertask_type(type_expression_for_masks.delta_task_type));
        cout << curriculum[i + 1]->to_string() << endl;
        memset_visited(vis_type, num_subtree_markers);
        BittreeProgram program = BittreeProgram(curriculum[i + 1], nullptr, num_subtree_markers, max_mask_type_depth, num_subtasks);
    }

//      BittreeTaskInstance instances = BittreeTaskInstance(curriculum[4]);

}

void BitvectorTasks::augment_subdomains(vector<MaskAndCost>& subdomains, BittreeTaskType* current_bittree, int num_prev_subtasks, int task_id)
{
    for (int subdomain_id = 0; subdomain_id < subdomains.size(); subdomain_id++) {
        vector<BittreeTaskType*> new_current_bittree = get_multi_task_type(new BittreeTypeExpression(task_name), task_id+1);
        BittreeTaskTypeAsPartialFunction* bittree_as_partial = new BittreeTaskTypeAsPartialFunction(
                new_current_bittree[new_current_bittree.size()-1], num_prev_subtasks);

        bittree_as_partial->assign_bits(subdomains[subdomain_id]);
        bittree_as_partial->update_bitvector();

        subdomains[subdomain_id].set_now_bittree_as_partial(bittree_as_partial);
        cout <<"HERE::" << subdomains[subdomain_id].now_canvas->to_string__one_line() << endl;
    }
    for (int subdomain_id = 0; subdomain_id < subdomains.size(); subdomain_id++) {
        cout <<"HERE::" << subdomains[subdomain_id].now_canvas->to_string__one_line() << endl;
    }
}

void BitvectorTasks::delta_wiring(vector<MaskAndCost> &subdomains, BittreeTaskType *task_type, int task_id,
                                  vector<MaskAndCost> &next_subdomains, string init_language_name,
                                  BittreeTaskType *next_task_type, vector<MaskAndCost> &prev_subdomains,
                                  vector<MaskAndCost> &new_prev_subdomains)
{

    augment_subdomains(subdomains, task_type, num_prev_subtasks, task_id);

    next_subdomains = get_next_subdomains(
            metric, dir_path, init_language_name,
            subdomains, task_type, next_task_type, num_prev_subtasks, task_id);


    ofstream mask_propagation_fout(
            dir_path + "/mask_propagation_model__task_id_" + std::to_string(task_id + 1));


    if(prev_subdomains.size() >= 1) {
        for (int i = 0; i < prev_subdomains.size(); i++) {
            mask_propagation_fout << prev_subdomains[i].to_string() << endl;
            for (int j = 0; j < prev_subdomains[i].local_variety.size(); j++) {
                mask_propagation_fout << "\t" << prev_subdomains[i].local_variety[j].to_string() << endl;
            }
        }
        mask_propagation_fout << endl;

        for (int i = 0; i < subdomains.size(); i++) {
            mask_propagation_fout << subdomains[i].to_string() << endl;
        }
        mask_propagation_fout << endl;
        mask_propagation_fout << endl;

        for (int now_id = 0; now_id < subdomains.size(); now_id++) {
            mask_propagation_fout << subdomains[now_id].now_canvas->to_string__one_line() << " :: ";
            for (int prev_id = 0; prev_id < prev_subdomains.size(); prev_id++) {
                for (int edge_id = 0; edge_id < prev_subdomains[prev_id].local_variety.size(); edge_id++) {
                    if (subdomains[now_id] == prev_subdomains[prev_id].local_variety[edge_id]) {
                        mask_propagation_fout <<
                                              prev_subdomains[prev_id].now_canvas->to_string__one_line()
                                              << " -> " <<
                                              prev_subdomains[prev_id].next_canvas->to_string__one_line()
                                              << " then " <<
                                              prev_subdomains[prev_id].local_variety[edge_id].program->AutomatonRule::to_string()
                                              << " | ";
                    }
                }
            }
            mask_propagation_fout << endl;
        }
        mask_propagation_fout << endl;

        for (int now_id = 0; now_id < subdomains.size(); now_id++) {
            mask_propagation_fout << subdomains[now_id].now_canvas->to_string__one_line() << " :: ";
            pair<int, pair<int, int> > best_mask = make_pair(-1, make_pair(-1, -1));
            for (int prev_id = 0; prev_id < prev_subdomains.size(); prev_id++) {
                for (int edge_id = 0; edge_id < prev_subdomains[prev_id].local_variety.size(); edge_id++) {
                    if (subdomains[now_id] == prev_subdomains[prev_id].local_variety[edge_id]) {
                        if(best_mask.first == -1)
                        {
                            assert(prev_subdomains[prev_id].local_variety[edge_id].cost>=0);
                            best_mask = make_pair(prev_subdomains[prev_id].local_variety[edge_id].cost,
                                                  make_pair(prev_id, edge_id));
                        }
                        else
                        {
                            best_mask = min(best_mask, make_pair(prev_subdomains[prev_id].local_variety[edge_id].cost,
                                                                 make_pair(prev_id, edge_id)));
                        }
                    }
                }
            }

            if(best_mask.first != -1)
            {
                int prev_id = best_mask.second.first;
                int edge_id = best_mask.second.second;
                mask_propagation_fout <<
                                      prev_subdomains[prev_id].now_canvas->to_string__one_line()
                                      << " -> " <<
                                      prev_subdomains[prev_id].next_canvas->to_string__one_line()
                                      << " then " <<
                                      prev_subdomains[prev_id].local_variety[edge_id].program->AutomatonRule::to_string()
                                      << " | ";
                subdomains[now_id].set_best_edge(&prev_subdomains[prev_id], edge_id);
                if(prev_subdomains[prev_id].best_edge.first != nullptr)
                {
                    mask_propagation_fout << "prev_best_edge:: " <<
                                          prev_subdomains[prev_id].best_edge.first->now_canvas->to_string__one_line()
                                          << " -> " <<
                                          prev_subdomains[prev_id].best_edge.first->next_canvas->to_string__one_line()
                                          << " then " <<
                                          prev_subdomains[prev_id].best_edge.first->local_variety[prev_subdomains[prev_id].best_edge.second].program->AutomatonRule::to_string()
                                          << " | ";

                }
            }

            mask_propagation_fout << endl;
        }

        mask_propagation_fout.close();
    }

    new_prev_subdomains = subdomains;

}

void remove_duplicates(vector<vector<MaskAndCost> > & masks_of_task_id)
{
    vector<vector<MaskAndCost> > ret_masks_of_task_id;
    set<Bitvector> masks;
    for(int i = 0;i<masks_of_task_id.size();i++)
    {
        ret_masks_of_task_id.push_back(vector<MaskAndCost>());
        for(int j = 0;j<masks_of_task_id[i].size();j++)
        {
            if(masks.find(masks_of_task_id[i][j]) == masks.end())
            {
                ret_masks_of_task_id[i].push_back(masks_of_task_id[i][j]);
                masks.insert((Bitvector)masks_of_task_id[i][j]);
            }
        }
    }
    masks_of_task_id = ret_masks_of_task_id ;
}

void BitvectorTasks::one_step_of_incremental_meta_generalization(
        bool is_first,
        int task_id,
        vector<MetaExample> meta_examples_of_task_id,
        vector<InstanceTree*> inst_trees_of_task_id,
        vector<MaskAndCost> &next_subdomains,
        vector<vector<MaskAndCost> > masks_of_task_id,
        BittreeTaskType *task_type,
        BittreeTaskType *next_task_type,
        vector<MaskAndCost> &prev_subdomains,
        vector<MaskAndCost> &new_prev_subdomains,
        vector<MetaExample> prev_train_set,
        vector<InstanceTree*> prev_inst_trees,

        vector<MetaExample> & ret_train_meta_examples,
        vector<InstanceTree*> &ret_train_instsnace_subtrees,
        ReasoningSchemaOptimizer* & ret_reasoning_schema
)
{
    assert(masks_of_task_id.size() == 0);
    assert(ret_train_meta_examples.size() == 0);
    assert(ret_train_instsnace_subtrees.size() == 0);
    assert(ret_reasoning_schema == nullptr);

    assert(next_subdomains.size() != 0 || is_first);
    if (mode == progressive_prior_mode && next_subdomains.size() != 0) {
        cout << "with_alternative:" << endl;

        for (int i = 0; i < next_subdomains.size(); i++) {
            cout << next_subdomains[i].to_string() << endl;
        }
        cout << endl;

        bool more_buckets = true;
        if(more_buckets) {
            int prev_cost = -1;
            vector<MaskAndCost> local_subdomains;
            for (int i = 0; i < next_subdomains.size(); i++) {
                if (prev_cost != next_subdomains[i].cost && prev_cost != -1) {
                    masks_of_task_id.push_back(local_subdomains);
                    local_subdomains.clear();
                }
                local_subdomains.push_back(next_subdomains[i]);
                prev_cost = next_subdomains[i].cost;
            }
        }
        else
        {
            masks_of_task_id.push_back(next_subdomains);
        }

        ofstream out_next_subdomains(dir_path + "/current_subdomains__task_id_"+std::to_string(task_id+1));
        for(int i = 0;i<next_subdomains.size(); i++)
        {
            out_next_subdomains << next_subdomains[i].to_string() << " " << next_subdomains[i].cost <<endl;
        }
        out_next_subdomains.close();

    }
//            else {
    task_type->to_meta_example(-1, num_prev_subtasks).append_to_masks(
            min_mask_size, max_mask_size, num_first_in_prior, masks_of_task_id);
//            }

    remove_duplicates(masks_of_task_id);

    string language_name =
            "[task_id=" + std::to_string(task_id + 1) + "]";

    assert(meta_examples_of_task_id.size() > 0);
    assert(masks_of_task_id.size() > 0);
    assert(masks_of_task_id[0].size() > 0);

    int len_domain_of_meta_example = meta_examples_of_task_id[0].get_function_size();
    int len_mask = masks_of_task_id[0][0].get_size();

    cout << meta_examples_of_task_id[0].partial_function.to_string__one_line() << endl;
    cout << meta_examples_of_task_id[0].partial_function.to_string() << endl;
    cout << meta_examples_of_task_id[0].to_string() << endl;
    cout << masks_of_task_id[0][0].to_string() << endl;
    assert(len_domain_of_meta_example == len_mask);

    for (int j = 0; j < masks_of_task_id.size(); j++) {
        for (int k = 0; k < masks_of_task_id[j].size(); k++) {
            cout << bitvector_to_str(masks_of_task_id[j][k], masks_of_task_id[j][k].get_size()) << endl;
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


    //Build Maps between meta_examples and superinstance_trees
    vector<InstanceTree*> superinstance_tree = std::move(inst_trees_of_task_id);
    vector<MetaExample> meta_examples = meta_examples_of_task_id;

    assert(superinstance_tree.size() == meta_examples.size());

    map<MetaExample, InstanceTree*> meta_examples_to_instance_trees;
    map<InstanceTree*, MetaExample> instnace_tree_to_meta_examples;

    for(int i = 0;i<meta_examples.size();i++)
    {
        assert(meta_examples_to_instance_trees.find(meta_examples[i]) == meta_examples_to_instance_trees.end());
        meta_examples_to_instance_trees[meta_examples[i]] = superinstance_tree[i];
        assert(instnace_tree_to_meta_examples.find(superinstance_tree[i]) == instnace_tree_to_meta_examples.end());
        instnace_tree_to_meta_examples[superinstance_tree[i]] = meta_examples[i];
    }

    if (train_set_minimization && !is_first) {

        vector<MetaExample> test_meta_examples = meta_examples_of_task_id;
        vector<MetaExample> train_meta_examples;

        bool random_train_set = false;
        if(random_train_set) {
            int local_seed_train_set_size = seed_train_set;
            if (local_seed_train_set_size == -1) {
                local_seed_train_set_size = test_meta_examples.size();
            } else {
                std::shuffle(test_meta_examples.begin(), test_meta_examples.end(),
                             std::mt19937(std::random_device()()));
            }

            for (int i = 0; i < test_meta_examples.size(); i++) {
                test_meta_examples[i].idx = i;
            }

            for (int i = 0; i < min((int) test_meta_examples.size(), local_seed_train_set_size); i++) {
                train_meta_examples.push_back(test_meta_examples[i]);
                train_meta_examples.back().idx = (int) train_meta_examples.size() - 1;
            }
        }
//        else
//        {


            for(int i = 0; i<prev_inst_trees.size();i++)
            {
                for(int j = 0;j<prev_inst_trees[i]->superinstance_trees_size;j++)
                {
                    InstanceTree* superinstance = prev_inst_trees[i]->superinstance_trees[j];
                    assert(superinstance != nullptr);
                    assert(instnace_tree_to_meta_examples.find(superinstance) != instnace_tree_to_meta_examples.end());
                    train_meta_examples.push_back(instnace_tree_to_meta_examples[superinstance]);
                    train_meta_examples[train_meta_examples.size()-1].idx = (int)train_meta_examples.size()-1;
                }
            }




//        }
        string init_language_name = language_name;

        vector<MaskAndCost> subdomains;
        int min_train_set_size = test_meta_examples.size();
        vector<MetaExample> min_train_meta_examples;

        summary << "task_id " << task_id + 1 << endl;
        summary_with_times << "task_id_" << task_id + 1 << " ";

        for (int minimization_step = 0; minimization_step < num_minimization_steps; minimization_step++) {
            bool all_solved = false;
            vector<MaskAndCost> local_subdomains;
            while (!all_solved) {
                language_name = init_language_name + "__min_step=" + std::to_string(minimization_step) +
                                "__size_of_train_set=" +
                                std::to_string(train_meta_examples.size());

                ReasoningSchemaOptimizer * my_reasoning_schema = new
                        ReasoningSchemaOptimizer(
                                train_meta_examples, language_name, masks_of_task_id, dir_path, metric);

                all_solved = true;
                int num_added = 0;
                for (int i = 0; i < test_meta_examples.size(); i++) {
                    PartialFunction generalization = my_reasoning_schema->query(test_meta_examples[i].partial_function);
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
                    } else {
                        assert(test_meta_examples[i].generalization.is_contained_in(generalization));
                        cout << "ok" << endl;
                    }
                }

                if (all_solved) {
                    local_subdomains = my_reasoning_schema->get_subdomains();
                    ret_reasoning_schema = my_reasoning_schema;
                    ret_train_meta_examples = train_meta_examples;
                }

                if (train_meta_examples.size() >= min_train_set_size) {
                    break;
                }
            }
            if (all_solved) {
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

            std::shuffle(train_meta_examples.begin(), train_meta_examples.end(),
                         std::mt19937(std::random_device()()));

            vector<MetaExample> new_train_set;

            new_train_set.reserve(train_meta_examples.size() * minimization_fraction);
            for (int i = 0; i < train_meta_examples.size() * minimization_fraction; i++) {
                new_train_set.emplace_back(train_meta_examples[i]);
                new_train_set[i].idx = i;
            }
            train_meta_examples = new_train_set;
        }

        next_subdomains.clear();

        delta_wiring(subdomains, task_type, task_id, next_subdomains, init_language_name, next_task_type,
                     prev_subdomains, new_prev_subdomains);

    } else {

        if (true) {
            vector<MetaExample> local_meta_examples = meta_examples_of_task_id;
            int prev_meta_examples_size = -1;
            int now_meta_examples_size = (int) local_meta_examples.size();
            int rec_id = 0;

            vector<MaskAndCost> subdomains;

            string init_language_name = language_name;

            do {
                prev_meta_examples_size = now_meta_examples_size;

                language_name = language_name + "__rec=" + std::to_string(rec_id);

                ReasoningSchemaOptimizer * my_reasoning_schema = new
                        ReasoningSchemaOptimizer(
                                local_meta_examples, language_name, masks_of_task_id, dir_path, metric);

                for (int i = 0; i < local_meta_examples.size(); i++) {
                    PartialFunction generalization = my_reasoning_schema->query(local_meta_examples[i].partial_function);
                    cout << "query  " << local_meta_examples[i].to_string() << endl;
                    cout << "result " << generalization.to_string() << endl;
                    cout << endl;
                    assert(generalization.is_contained_in(local_meta_examples[i].generalization));
                    assert(local_meta_examples[i].generalization.is_contained_in(generalization));
                }
                cout << "TESTING DONE. ALL CORRECT" << endl;

                if (rec_id == 0) {
                    subdomains = my_reasoning_schema->get_subdomains();
                    ret_reasoning_schema = my_reasoning_schema;
                    ret_train_meta_examples = local_meta_examples;
                }

                local_meta_examples = my_reasoning_schema->get_necessary_meta_examples(false);

                now_meta_examples_size = (int) local_meta_examples.size();

                rec_id++;
                if (recursive_rep_set_depth != -1 && rec_id > recursive_rep_set_depth) {
                    break;
                }
            } while (now_meta_examples_size != prev_meta_examples_size);

            next_subdomains.clear();

            delta_wiring(subdomains, task_type, task_id, next_subdomains, init_language_name, next_task_type,
                         prev_subdomains, new_prev_subdomains);

        } else if (true) {
            ReasoningSchemaOptimizer my_reasoning_schema =
                    ReasoningSchemaOptimizer(
                            meta_examples_of_task_id, language_name, masks_of_task_id, dir_path, metric);

            for (int j = 0; j < meta_examples_of_task_id.size(); j++) {
                PartialFunction generalization = my_reasoning_schema.query(meta_examples_of_task_id[j].partial_function);
                cout << "query  " << meta_examples_of_task_id[j].to_string() << endl;
                cout << "result " << generalization.to_string() << endl;
                cout << endl;
                assert(generalization.is_contained_in(meta_examples_of_task_id[j].generalization));
            }
            cout << "TESTING DONE. ALL CORRECT" << endl;

            vector<MetaExample> necessary_meta_examples = my_reasoning_schema.get_necessary_meta_examples(false);
        } else {
            cout << "Need to send a prior over Bitmasks as a vector of vectors of bitvectors" << endl;
            assert(false);
            TraceVersionSpace trace_version_space = TraceVersionSpace(meta_examples_of_task_id,
                                                                      masks_of_task_id[0]);
        }
    }
    for(int i = 0;i<ret_train_meta_examples.size();i++)
    {
        assert(meta_examples_to_instance_trees.find(ret_train_meta_examples[i]) != meta_examples_to_instance_trees.end());
        ret_train_instsnace_subtrees.push_back(meta_examples_to_instance_trees[ret_train_meta_examples[i]]);
    }
}

BitvectorTasks::BitvectorTasks(Task *_task_name,
                               int _init_iter,
                               int _num_iter,
                               int _recursive_rep_set_depth,
                               MetricType _metric,
                               ModeType _mode,
                               int _min_mask_size,
                               int _max_mask_size,
                               int _num_prev_subtasks,
                               string _dir_path,
                               int _num_first_in_prior,
                               bool _train_set_minimization,
                               int _seed_train_set,
                               int _num_minimization_steps,
                               double _minimization_fraction)
{
    task_name = _task_name;
    init_iter = _init_iter;
    num_iter = _num_iter;
    recursive_rep_set_depth = _recursive_rep_set_depth;
    metric = _metric;
    mode = _mode;
    min_mask_size = _min_mask_size;
    max_mask_size = _max_mask_size;
    num_prev_subtasks = _num_prev_subtasks;
    dir_path = _dir_path;
    num_first_in_prior = _num_first_in_prior;
    train_set_minimization = _train_set_minimization;
    seed_train_set = _seed_train_set;
    num_minimization_steps = _num_minimization_steps;
    minimization_fraction = _minimization_fraction;

    //set up type expression
    BittreeTypeExpression type_expression_for_meta_examples = BittreeTypeExpression(task_name);
    BittreeTypeExpression type_expression_for_multi_task_set = BittreeTypeExpression(task_name);

    vector<BittreeTaskType *> multi_task_type = get_multi_task_type(&type_expression_for_multi_task_set, num_iter);

    vector<vector<MetaExample> > meta_examples;
    vector<vector<InstanceTree*> > instance_subtrees;
    get_meta_examples(&type_expression_for_meta_examples, task_name, num_iter, num_prev_subtasks, meta_examples, instance_subtrees);

    vector<MaskAndCost> next_subdomains;
    vector<vector<MaskAndCost> > prev_subdomains;
    prev_subdomains.push_back(vector<MaskAndCost>());

    vector<vector<vector<MaskAndCost> > > masks =
            vector<vector<vector<MaskAndCost> > >(meta_examples.size(), vector<vector<MaskAndCost> >());

    set_up_directory();

    summary = ofstream(dir_path + "/summary");
    summary_with_times = ofstream(dir_path + "/summary_with_times");

    init_time = time(nullptr);
    
    vector<vector<MetaExample> > rep_set_per_task = vector<vector<MetaExample> >(meta_examples.size(), vector<MetaExample>());
    vector<vector<InstanceTree*> > inst_tree_per_task = vector<vector<InstanceTree*> >(meta_examples.size(), vector<InstanceTree*>());

    for(int task_id = init_iter, is_first = true, iter = 0; task_id < meta_examples.size(); task_id++, is_first = false, iter++) {
        BittreeTaskType * next_task_type = nullptr;
        if(task_id + 1 < multi_task_type.size())
        {
            next_task_type = multi_task_type[task_id+1];
        }

        prev_subdomains.push_back(vector<MaskAndCost>());
        ReasoningSchemaOptimizer * reasoning_schema = nullptr;

        vector<MetaExample> prev_rep_set;
        vector<InstanceTree*> prev_inst_tree;
        if(task_id>=1)
        {
            prev_rep_set = rep_set_per_task[task_id-1];
            prev_inst_tree = inst_tree_per_task[task_id-1];
        }

        one_step_of_incremental_meta_generalization(
                is_first,
                task_id,
                meta_examples[task_id],
                instance_subtrees[task_id],
                next_subdomains,
                masks[task_id],
                multi_task_type[task_id],
                next_task_type,
                prev_subdomains[iter],
                prev_subdomains[iter + 1],

                prev_rep_set,
                prev_inst_tree,

                rep_set_per_task[task_id],
                inst_tree_per_task[task_id],
                reasoning_schema
            );


        ofstream fout_inst_trees(dir_path + "/inst_trees__task_id_"+std::to_string(task_id+1));

        for(int i = 0;i<inst_tree_per_task[task_id].size();i++)
        {
            fout_inst_trees << inst_tree_per_task[task_id][i]->instance->to_string__one_line(num_prev_subtasks) << " | ";
            for(int j = 0;j<inst_tree_per_task[task_id][i]->num_superinstances;j++)
            {
                fout_inst_trees << "superinst[" + std::to_string(j) +"] " + inst_tree_per_task[task_id][i]->superinstances[j]->to_string__one_line(num_prev_subtasks) << "\t";
            }
            fout_inst_trees << endl;
        }

        fout_inst_trees.close();

        vector<Module*> modules = reasoning_schema->get_modules();

        ofstream module_meta_examples(dir_path + "/module_meta_examples__task_id_"+std::to_string(task_id+1));

        for(int i = 0;i<modules.size();i++)
        {
            vector<MetaExample> necessary_meta_examples = modules[i]->module_meta_examples;
            for(int j = 0;j<necessary_meta_examples.size();j++)
            {
                module_meta_examples << necessary_meta_examples[j].to_string() << endl;
            }
            module_meta_examples << endl;
        }
        module_meta_examples.close();

        vector<Module*> program_as_modules = reasoning_schema->get_program_as_vector_of_modules();

        ofstream program_as_modules_out(dir_path + "/program_as_modules__task_id_"+std::to_string(task_id+1));

        for(int i = 0;i<program_as_modules.size();i++)
        {
            vector<MetaExample> necessary_meta_examples = program_as_modules[i]->module_meta_examples;
            for(int j = 0;j<necessary_meta_examples.size();j++)
            {
                program_as_modules_out << necessary_meta_examples[j].to_string() << endl;
            }
            program_as_modules_out << endl;
        }

        program_as_modules_out.close();

        ofstream rep_meta_examples(dir_path + "/rep_meta_examples__task_id_"+std::to_string(task_id+1));

        for(int j = 0;j<rep_set_per_task[task_id].size();j++)
        {
            rep_meta_examples << rep_set_per_task[task_id][j].to_string() << endl;
        }
        rep_meta_examples << endl;

        rep_meta_examples.close();

        ofstream imp_meta_examples(dir_path + "/imp_meta_examples__task_id_"+std::to_string(task_id+1));

        vector<MetaExample> imp_set = reasoning_schema->get_necessary_meta_examples(false);
        for(int j = 0;j<imp_set.size();j++)
        {
            imp_meta_examples << imp_set[j].to_string() << endl;
        }
        imp_meta_examples << endl;

        imp_meta_examples.close();
    }
}

void BitvectorTasks::set_up_directory() {
    //set up directory
    dir_path =
            "task_name=" + task_name->get_task_name() +
            "-gen=67.2-init_iter=" + std::to_string(init_iter) +
            "-end_iter=" + std::to_string(num_iter) +
            "-num_prev_subtasks=" + std::to_string(num_prev_subtasks) +
            "-mask_size=[" +std::to_string(min_mask_size) + "," +std::to_string(max_mask_size) + "]" +
            "-fst_from_pior="+std::to_string(num_first_in_prior) +
            "-metric=" + metric_type_name[metric]+
            "-mode="+mode_type_name[mode]+
            "-tsm="+std::to_string(train_set_minimization)[0]+
            "-seed_train_set=" + std::to_string(seed_train_set)+
            "-num_minimize_steps="+std::to_string(num_minimization_steps)+
            "-minimize_fraction="+std::to_string(minimization_fraction);

    int check_mkdir = mkdir( dir_path.c_str(), 0777);
    if (check_mkdir == -1){
        cerr << "Error :  " << strerror(errno) << endl;
        assert(false);
    }
    else {
        cout << "Directory created: "  << dir_path << endl;
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

//
// Created by Kliment Serafimov on 1/8/20.
//

#include "util.h"
#include "BitvectorTasks.h"
#include "TraceVersionSpace.h"

#include <iostream>

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

InstanceTree::InstanceTree(BittreeTaskType* _instance, BittreeTaskType* _delta, TaskName _task_name) {
    instance = _instance;
    delta = _delta;
    task_name = _task_name;
}

void InstanceTree::prepare_for_deepening()
{
    superinstance_type = instance->get_supertask_type(delta);

    vector<BitInBittree*> input_bits;

    memset_visited(vis_bits);
    superinstance_type->input->append_bits(input_bits);

    int num_input_bits = 0;
    for(int i = 0;i<input_bits.size();i++)
    {
        if(!input_bits[i]->is_bit_set)
        {
            num_input_bits += 1;
        }
    }

    num_superinstances = (1 << num_input_bits);

    assert(num_superinstances <= max_instance_tree_degree);

    for(int i = 0; i < num_superinstances; i++)
    {
        superinstances[i] = new BittreeTaskType(
                NULL, Name("superinstances", i), superinstance_type, true);
        superinstances[i]->subtask = superinstance_type->subtask;
//        cout << "---------------------------------------------------" << endl;
//        cout <<superinstances[i]->to_string() << endl;
        vector<BitInBittree*> local_input_bits;
        memset_visited(vis_bits);
       superinstances[i]->input->append_bits(local_input_bits);
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

void InstanceTree::deepen()
{
    if(deepened)
    {
        for (int i = 0; i < num_superinstances; i++) {
            superinstance_trees[i]->deepen();
        }
    }
    else
    {
        if(!prepared_for_deepening)
        {
            prepare_for_deepening();
        }
        assert(superinstance_trees.size() == 0);
        for (int i = 0; i < num_superinstances; i++) {
            superinstance_trees.push_back(
                    new InstanceTree(superinstances[i], delta, task_name));
            superinstance_trees[i]->prepare_for_deepening();
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
    bittree_task_type->input->append_bits(input_bits);
    bittree_task_type->output->append_bits(output_bits);

    int num_input_bits = input_bits.size();
    int num_instances = (1<<num_input_bits);
    for(int i = 0; i<num_instances; i++)
    {
        instances.push_back(BittreeTaskType(NULL, Name("instances", instances.size()), bittree_task_type, true));
        vector<BitInBittree*> local_input_bits;
        memset_visited(vis_bits);
        instances[i].input->append_bits(local_input_bits);
        assert(local_input_bits.size() == num_input_bits);
        for(int j = 0;j<local_input_bits.size();j++)
        {
            local_input_bits[j]->is_bit_set = true;
            local_input_bits[j]->bit_val = get_bit(i, j);
        }
//        cout << instances[i].to_string() << endl;
    }
}

vector<vector<Bitvector> > BitvectorTasks::masks_generator(
        BittreeTypeExpression* type_expression, int init_num_iter)
{
    int num_iter = init_num_iter;

    if(false)
    {
        int max_masks_size = 3;
//        vector<Bitvector> masks =
//                meta_examples[i][0].get_masks(max_mask_size);

        vector<vector<Bitvector> > masks;

        vector<BittreeTaskType *> curriculum;
        curriculum.push_back(type_expression->base_task_type);
//        masks.push_back(curriculum[0]->to_meta_example(-1).get_masks(max_masks_size));
//        num_iter--;
        for (int i = 0; i < num_iter; i++) {
            curriculum.push_back(curriculum[i]->get_supertask_type(type_expression->delta_task_type));
            cout << curriculum[i + 1]->to_string() << endl;
            masks.push_back(curriculum[i+1]->to_meta_example(-1).get_masks(max_masks_size));
        }

        return masks;

    }
    else {
        vector<vector<Bitvector> > masks;

        vector<BittreeTaskType *> curriculum;
        int num_subtree_markers = 10;
        int max_mask_type_depth = 10;
        curriculum.push_back(type_expression->base_task_type);

//        memset_visited(vis_type, num_subtree_markers);
//        BittreeProgram program = BittreeProgram(curriculum[0], NULL, num_subtree_markers);
//        masks.push_back(program.all_bittree_masks_as_bitvectors);
//        num_iter--;
        for (int i = 0; i < num_iter; i++) {
            curriculum.push_back(curriculum[i]->get_supertask_type(type_expression->delta_task_type));
//            cout << curriculum[i + 1]->to_string() << endl;
            memset_visited(vis_type, num_subtree_markers);
            BittreeProgram program = BittreeProgram(curriculum[i + 1], NULL, num_subtree_markers, max_mask_type_depth);
            masks.push_back(program.all_bittree_masks_as_bitvectors);
        }

//      BittreeTaskInstance instances = BittreeTaskInstance(curriculum[4]);

        return masks;
    }

}

vector<vector<MetaExample> >
BitvectorTasks::get_meta_examples(BittreeTypeExpression *type_expression, TaskName task_name, int init_num_iter,
                                  int subtask_depth)
{
    type_expression->base_task_type->solve(task_name);
//    cout << type_expression->base_task_type->to_string() << endl;

    int num_iter = init_num_iter;
    InstanceTree instances = InstanceTree(type_expression->base_task_type, type_expression->delta_task_type, task_name);
    instances.prepare_for_deepening();
    num_iter--;

    for(int iter = 0;iter<num_iter;iter++)
    {
        vector<vector<MetaExample> > meta_examples;
        instances.populate_meta_examples(meta_examples, 0, subtask_depth);
//        for(int i = 0;i<meta_examples.size();i++)
//        {
//            for(int j = 0;j<meta_examples[i].size();j++)
//            {
//                cout << meta_examples[i][j].to_string() << endl;
//            }
//            cout << endl;
//        }
//        cout << "----------------------------------------" << endl;

        if(iter < num_iter)
        {
            instances.deepen();
        }
    }

    vector<vector<MetaExample> > meta_examples;
    instances.populate_meta_examples(meta_examples, 0, subtask_depth);

    assert(meta_examples.size() == init_num_iter);

    return meta_examples;
}


BitvectorTasks::BitvectorTasks()
{
    string str_task_name;

    str_task_name = str_task_name__sum;
//    str_task_name = str_task_name__greater;
//    str_task_name = str_task_name__cumulative_binary_operator;
//    str_task_name = str_task_name__bitwise_binary_operator;
//    str_task_name = str_task_name__one_shift_idx; // requires double_node
//    str_task_name = str_task_name__count_unary; // requires double_node
//    str_task_name = str_task_name__unary_sum; // requires double_node
//    str_task_name = str_task_name__least_set_bit;
//    str_task_name = str_task_name__max_window_between_bits;
    TaskName task_name = TaskName(str_task_name);

//    str_task_name = str_task_name__multiply_by;
//    int multiply_by = 3;
//    TaskName task_name = TaskName(str_task_name, multiply_by);

//    str_task_name = str_task_name__one_shift_idx__reverse_subtask;
//    str_task_name = str_task_name__count_unary__reverse_subtask;
//    int init_size = 3;
//    TaskName task_name = TaskName(str_task_name, init_size);

    if(false)
    {
        BittreeTypeExpression type_expression_for_masks = BittreeTypeExpression(task_name);
        vector<BittreeTaskType*> curriculum;
        int num_iter = 4;
        int num_subtree_markers = 1;
        int max_mask_type_depth = 1;
        curriculum.push_back(type_expression_for_masks.base_task_type);

        memset_visited(vis_type, num_subtree_markers);
        BittreeProgram program = BittreeProgram(curriculum[0], NULL, num_subtree_markers, max_mask_type_depth);
        for (int i = 0; i < num_iter; i++) {
            curriculum.push_back(curriculum[i]->get_supertask_type(type_expression_for_masks.delta_task_type));
            cout << curriculum[i + 1]->to_string() << endl;
            memset_visited(vis_type, num_subtree_markers);
            BittreeProgram program = BittreeProgram(curriculum[i + 1], NULL, num_subtree_markers, max_mask_type_depth);
        }

//      BittreeTaskInstance instances = BittreeTaskInstance(curriculum[4]);

    }
    else
    {
        BittreeTypeExpression type_expression_for_meta_examples = BittreeTypeExpression(task_name);
        BittreeTypeExpression type_expression_for_masks = BittreeTypeExpression(task_name);

        //parameter
        int num_iter = 4;

        if(task_name.num_iter_defined)
        {
            num_iter = task_name.num_iter;
        }

        int num_prev_subtasks = -1;

        vector<vector<Bitvector> > masks = masks_generator(&type_expression_for_masks, num_iter);

        vector<vector<MetaExample> > meta_examples = get_meta_examples(
                &type_expression_for_meta_examples, task_name, num_iter, num_prev_subtasks);

        assert(meta_examples.size() == masks.size());

        for(int i = 0;i<meta_examples.size();i++) {

            //parameter
            int max_mask_size = 3;

            string language_name =
                    task_name.get_task_name() +
                    "[third_gen][" +
                    "num_prev_subtasks=" + std::to_string(num_prev_subtasks) +
//                    ",max_wire_width="+ std::to_string(max_mask_size) +
                    ",size="+std::to_string(i+1) + "]";

            assert(meta_examples[i].size() >= 0);

//            vector<Bitvector> masks =
//                    meta_examples[i][0].get_masks(max_mask_size);

            for(int j = 0;j<masks[i].size();j++)
            {
                cout << bitvector_to_str(masks[i][j], masks[i][j].get_size()) << endl;
            }

            //ideas:
            //multi-objective beam search
            //  objectives include num meta examples necessary to program, node degree, num bits modeled, num instances modeled, primitive size
            //  intermediate bits of computation are those that segment the dataset such that single operators can now apply to a wider domain.
            //  this leads to emergent task decomposition
            //  optimize for task decomposition description in various cost semantics; type size, runtime, memory, depth of compilation.

            //the ordering based on the structured mask ordering can be used to discover the type (init and delta)

            //project the ordering of the subdomain masks onto the language of f(n) = ordering over the subdomain masks that solves the problem. 

            if(false) {
                MinimalFactoringSchema my_schema =
                        MinimalFactoringSchema(
                                meta_examples[i], language_name, masks[i]);

                for (int j = 0; j < meta_examples[i].size(); j++) {
                    PartialFunction generalization = my_schema.query(meta_examples[i][j].partial_function);
                    cout << "query  " << meta_examples[i][j].to_string() << endl;
                    cout << "result " << generalization.to_string() << endl;
                    cout << endl;
                    assert(generalization.is_contained_in(meta_examples[i][j].generalization));
                }
                cout << "TESTING DONE. ALL CORRECT" << endl;

                vector<MetaExample> necessary_meta_examples = my_schema.get_necessary_meta_examples(false);
            }
            else
            {
                TraceVersionSpace trace_version_space =
                        TraceVersionSpace(meta_examples[i], masks[i]);
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
//LatticeCompressor: SubdomainLattice -> program that when executed generates a trace (where trace: input->output path) in StateLattice

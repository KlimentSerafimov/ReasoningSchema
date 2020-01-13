//
// Created by Kliment Serafimov on 1/8/20.
//

#include "BitvectorTasks.h"

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
        PartialFunction partial_function = PartialFunction(function_size, total_function, -1);
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



InstanceTree::InstanceTree(BittreeTaskType* _instance, DeltaBittreeTaskType* _delta, TaskName _task_name) {
    instance = _instance;
    delta = _delta;
    task_name = _task_name;
}

void InstanceTree::prepare_for_deepening()
{
    superinstance_type = instance->get_supertask_type(*delta);

    vector<BitInBittree*> input_bits;

    superinstance_type.input->append_bits(input_bits);

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
        superinstances[i] = BittreeTaskType(&superinstance_type, true);
        superinstances[i].subtask = superinstance_type.subtask;
        cout << "---------------------------------------------------" << endl;
        cout << superinstances[i].to_string() << endl;
        vector<BitInBittree*> local_input_bits;
        superinstances[i].input->append_bits(local_input_bits);
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
        cout << superinstances[i].to_string() << endl;
        superinstances[i].solve(task_name);
        cout << superinstances[i].to_string() << endl;
        cout << "---------------------------------------------------" << endl;
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
            superinstance_trees.push_back(new InstanceTree(&superinstances[i], delta, task_name));
            superinstance_trees[i]->prepare_for_deepening();
        }
        deepened = true;
    }
}

void InstanceTree::populate_meta_examples(vector<vector<MetaExample> > &ret, int at_depth)
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
                    superinstances[i].to_meta_example_of_subtask_decomposition(ret[at_depth].size()));
        }
    }
    if(deepened)
    {
        assert(superinstance_trees.size() == num_superinstances);
        for(int i = 0; i < superinstance_trees.size();i++)
        {
            superinstance_trees[i]->populate_meta_examples(ret, at_depth+1);
        }
    }
}

BittreeTaskInstance::BittreeTaskInstance(BittreeTaskType* _bittree_task_type)
{
    bittree_task_type = _bittree_task_type;

    vector<BitInBittree*> input_bits;
    vector<BitInBittree*> output_bits;

    bittree_task_type->input->append_bits(input_bits);
    bittree_task_type->output->append_bits(output_bits);

    int num_input_bits = input_bits.size();
    int num_instances = (1<<num_input_bits);
    for(int i = 0; i<num_instances; i++)
    {
        instances.push_back(BittreeTaskType(_bittree_task_type, true));
        vector<BitInBittree*> local_input_bits;
        instances[i].input->append_bits(local_input_bits);
        assert(local_input_bits.size() == num_input_bits);
        for(int j = 0;j<local_input_bits.size();j++)
        {
            local_input_bits[j]->is_bit_set = true;
            local_input_bits[j]->bit_val = get_bit(i, j);
        }
        cout << instances[i].to_string() << endl;
    }
}


BitvectorTasks::BitvectorTasks()
{
    string str_task_name;

//    str_task_name = "sum";
//    str_task_name = "greater";
//    str_task_name = "cumulative_binary_operator";
//    str_task_name = "bitwise_binary_operator";

//    TaskName task_name = TaskName(str_task_name);

    str_task_name = "multiply_by";
    int multiply_by = 9;
    TaskName task_name = TaskName(str_task_name, multiply_by);

    BittreeTaskType input_task_type(internal_node, internal_node);
    DeltaBittreeTaskType delta_bittree_task_type(internal_node, internal_node);

    if(task_name.do_sum_task_type) {

        input_task_type.input->node_type = internal_node;
        input_task_type.input->children->push_back(new BittreeTypeNode(input_task_type.input, internal_node));
        input_task_type.input->children->push_back(new BittreeTypeNode(input_task_type.input, internal_node));

        input_task_type.output->node_type = internal_node;
        input_task_type.output->children->push_back(
                new BittreeTypeNode(input_task_type.output, leaf_node, new_machine_bit));


        delta_bittree_task_type.delta_input->node_type = internal_node;
        delta_bittree_task_type.delta_input->children->push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children->push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children->at(0)->delta =
                new BittreeTypeNode(NULL, internal_node);
        delta_bittree_task_type.delta_input->children->at(0)->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, shared_blanko_bit));
        delta_bittree_task_type.delta_input->children->at(1)->delta =
                new BittreeTypeNode(NULL, internal_node);
        delta_bittree_task_type.delta_input->children->at(1)->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, shared_blanko_bit));

        delta_bittree_task_type.delta_output->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, new_blanko_bit));

        cout << input_task_type.to_string() << endl;
    }

    if
            (
            task_name.do_greater_task_type
            )
    {

        input_task_type.input->node_type = internal_node;
        input_task_type.input->children->push_back(new BittreeTypeNode(input_task_type.input, internal_node));
        input_task_type.input->children->push_back(new BittreeTypeNode(input_task_type.input, internal_node));

        input_task_type.output->node_type = internal_node;
        input_task_type.output->children->push_back(
                new BittreeTypeNode(input_task_type.output, leaf_node, new_machine_bit));


        delta_bittree_task_type.delta_input->node_type = internal_node;
        delta_bittree_task_type.delta_input->children->push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children->push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children->at(0)->delta =
                new BittreeTypeNode(NULL, internal_node);
        delta_bittree_task_type.delta_input->children->at(0)->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, shared_blanko_bit));
        delta_bittree_task_type.delta_input->children->at(1)->delta =
                new BittreeTypeNode(NULL, internal_node);
        delta_bittree_task_type.delta_input->children->at(1)->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, shared_blanko_bit));

        cout << input_task_type.to_string() << endl;
    }

    if(task_name.do_cummulative_binary_operator)
    {

        input_task_type.input->node_type = internal_node;
        input_task_type.input->children->push_back(new BittreeTypeNode(input_task_type.input, internal_node));

        input_task_type.output->node_type = internal_node;
        input_task_type.output->children->push_back(
                new BittreeTypeNode(input_task_type.output, leaf_node, new_machine_bit));


        delta_bittree_task_type.delta_input->node_type = internal_node;
        delta_bittree_task_type.delta_input->children->push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children->at(0)->delta =
                new BittreeTypeNode(NULL, internal_node);
        delta_bittree_task_type.delta_input->children->at(0)->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, shared_blanko_bit));

        cout << input_task_type.to_string() << endl;
    }

    if(task_name.do_bitwise_binary_operator)
    {
        input_task_type.input->node_type = internal_node;
        input_task_type.input->children->push_back(new BittreeTypeNode(input_task_type.input, internal_node));
        input_task_type.input->children->push_back(new BittreeTypeNode(input_task_type.input, internal_node));

        delta_bittree_task_type.delta_input->node_type = internal_node;
        delta_bittree_task_type.delta_input->children->push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children->push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children->at(0)->delta =
                new BittreeTypeNode(NULL, internal_node);
        delta_bittree_task_type.delta_input->children->at(0)->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, shared_blanko_bit));
        delta_bittree_task_type.delta_input->children->at(1)->delta =
                new BittreeTypeNode(NULL, internal_node);
        delta_bittree_task_type.delta_input->children->at(1)->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, shared_blanko_bit));

        delta_bittree_task_type.delta_output->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, new_blanko_bit));

        cout << input_task_type.to_string() << endl;
    }

    if(task_name.do_multiplication_by)
    {
        input_task_type.input->node_type = internal_node;
        input_task_type.input->children->push_back(new BittreeTypeNode(input_task_type.input, internal_node));

        input_task_type.output->node_type = internal_node;
        int buffer = -1;
        if(task_name.multiply_by <= 1)
        {
            buffer = 0;
        }
        else if(task_name.multiply_by <= 2)
        {
            buffer = 1;
        }
        else if(task_name.multiply_by <= 4)
        {
            buffer = 2;
        }
        else if(task_name.multiply_by <= 8)
        {
            buffer = 3;
        }
        else if(task_name.multiply_by <= 16)
        {
            buffer = 4;
        }
        for(int i = 0;i<buffer;i++){
            input_task_type.output->children->push_back(
                    new BittreeTypeNode(input_task_type.output, leaf_node, new_machine_bit));
        }

        delta_bittree_task_type.delta_input->node_type = internal_node;
        delta_bittree_task_type.delta_input->children->push_back(new DeltaBittreeType(internal_node));
        delta_bittree_task_type.delta_input->children->at(0)->delta =
                new BittreeTypeNode(NULL, internal_node);
        delta_bittree_task_type.delta_input->children->at(0)->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, shared_blanko_bit));

        delta_bittree_task_type.delta_output->delta->children->push_back(
                new BittreeTypeNode(NULL, leaf_node, new_blanko_bit));

        cout << "HERE" << input_task_type.to_string() << endl;
    }

    if(false)
    {
        BittreeTaskType curriculum[5];
        curriculum[0] = input_task_type;
        for (int i = 0; i < 4; i++) {
            curriculum[i + 1] = curriculum[i].get_supertask_type(delta_bittree_task_type);
            cout << curriculum[i + 1].to_string() << endl;
        }

//      BittreeTaskInstance instances = BittreeTaskInstance(&curriculum[4]);
    }
    else
    {
        input_task_type.solve(task_name);
        cout << input_task_type.to_string() << endl;
        InstanceTree instances = InstanceTree(&input_task_type, &delta_bittree_task_type, task_name);
        instances.prepare_for_deepening();
        int num_iter = 6;
        for(int iter = 0;iter<num_iter;iter++)
        {
            vector<vector<MetaExample> > meta_examples;
            instances.populate_meta_examples(meta_examples, 0);
            for(int i = 0;i<meta_examples.size();i++)
            {
                for(int j = 0;j<meta_examples[i].size();j++)
                {
                    cout << meta_examples[i][j].to_string() << endl;
                }
                cout << endl;
            }
            cout << "----------------------------------------" << endl;

            if(iter < num_iter)
            {
                instances.deepen();
            }
        }

        vector<vector<MetaExample> > meta_examples;
        instances.populate_meta_examples(meta_examples, 0);

        for(int i = 0;i<meta_examples.size();i++) {
            string language_name =
                    task_name.get_task_name()+"[size="+std::to_string(i+1)+", num_prev_subtasks=1]";

            vector<int> masks;

            MinimalFactoringSchema my_schema =
                    MinimalFactoringSchema(meta_examples[i], language_name, masks);

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
    }
}


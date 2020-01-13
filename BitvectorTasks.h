//
// Created by Kliment Serafimov on 1/8/20.
//

#ifndef COMPACTPOSET_BITVECTORTASKS_H
#define COMPACTPOSET_BITVECTORTASKS_H

#include "PartialFunction.h"
#include "MetaExample.h"
#include "util.h"
#include <iostream>
#include "MinimalFactoringSchema.h"
#include "BittreeTaskType.h"

static const int max_instance_tree_degree = 4;

class InstanceTree
{
public:
    BittreeTaskType* instance;
    DeltaBittreeTaskType* delta;

    bool prepared_for_deepening = false;
    BittreeTaskType superinstance_type;

    int num_superinstances;
    BittreeTaskType superinstances[max_instance_tree_degree];

    bool deepened = false;
    vector<InstanceTree*> superinstance_trees;

    InstanceTree(BittreeTaskType* _instance, DeltaBittreeTaskType* _delta) {
        instance = _instance;
        delta = _delta;
    }

    void prepare_for_deepening()
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
            superinstances[i].solve_sum();
            cout << superinstances[i].to_string() << endl;
            cout << "---------------------------------------------------" << endl;
        }
        prepared_for_deepening = true;
    }

    void deepen()
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
                superinstance_trees.push_back(new InstanceTree(&superinstances[i], delta));
                superinstance_trees[i]->prepare_for_deepening();
            }
            deepened = true;
        }
    }

    void populate_meta_examples(vector<vector<MetaExample> > &ret, int at_depth)
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
};

class BittreeTaskInstance
{
public:

    BittreeTaskType* initialization;
    DeltaBittreeType* delta;

    BittreeTaskType curriculum[5];

    BittreeTaskType* bittree_task_type;

    vector<BittreeTaskType> instances;

    BittreeTaskInstance(BittreeTaskType* _bittree_task_type)
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
};

class BitvectorTasks {

    int num_inputs;
    int function_size;

    vector<PartialFunction> ordering_over_boolean_functions;
    void populate_ordering_over_boolean_functions_with_small_sum();
    void populate_ordering_over_boolean_functions_with_bitwise_and();
    MetaExample get_meta_example(PartialFunction partial_function);

public:

    BitvectorTasks(int function_size, int task_id);
    
    
    BitvectorTasks()
    {
        BittreeTaskType sum_task_type(internal_node, internal_node);

        sum_task_type.input->node_type = internal_node;
        sum_task_type.input->children->push_back(new BittreeTypeNode(sum_task_type.input, internal_node));
        sum_task_type.input->children->push_back(new BittreeTypeNode(sum_task_type.input, internal_node));

        sum_task_type.output->node_type = internal_node;
        sum_task_type.output->children->push_back(new BittreeTypeNode(sum_task_type.output, leaf_node, new_machine_bit));

        DeltaBittreeTaskType delta_bittree_task_type(internal_node, internal_node);

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

        cout << sum_task_type.to_string() << endl;

        if(false)
        {
            BittreeTaskType curriculum[5];
            curriculum[0] = sum_task_type;
            for (int i = 0; i < 4; i++) {
                curriculum[i + 1] = curriculum[i].get_supertask_type(delta_bittree_task_type);
                cout << curriculum[i + 1].to_string() << endl;
            }

//            BittreeTaskInstance instances = BittreeTaskInstance(&curriculum[4]);
        }
        else
        {
            sum_task_type.solve_sum();
            cout << sum_task_type.to_string() << endl;
            InstanceTree instances = InstanceTree(&sum_task_type, &delta_bittree_task_type);
            instances.prepare_for_deepening();
            int num_iter = 4;
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

            for(int i = meta_examples.size()-1;i<meta_examples.size();i++) {
                string language_name =
                        "sum[size="+std::to_string(i+1)+", num_prev_subtasks=1]";

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
};

#endif //COMPACTPOSET_BITVECTORTASKS_H

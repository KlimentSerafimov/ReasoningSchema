//
// Created by Kliment Serafimov on 1/21/20.
//

#include "IncrementalTypeExpression.h"
#include "BittreeInputOutputType.h"
#include <iostream>
#include <vector>

using namespace std;

IncrementalTypeExpression::IncrementalTypeExpression(Task *task_name)
{
    task_name->generate_bittree_task_expression(this);

}

void IncrementalTypeExpression::build_input_type(int num_input_operands, int init_input_sizes[], int delta_input_size[])
{
    //input type expression
    for(int i = 0;i<num_input_operands;i++)
    {
        //init_input_operand_sizes == 0
        base_task_type->io->add_input_child(internal_node);

        BittreeNode* init_delta_task_type_input_child =
                init_delta_task_type->add_input_child(leaf_node);

        init_delta_task_type_input_child->set_delta(
                internal_node, init_input_sizes[i], shared_blanko_bit);

        BittreeNode* delta_task_type_input_child =
                delta_task_type->add_input_child(leaf_node);

        delta_task_type_input_child->set_delta(
                internal_node, delta_input_size[i], shared_blanko_bit);


    }
}

void IncrementalTypeExpression::build_output_type(int num_outputs, int* init_output_sizes, int* delta_output_size) const
{
    for(int i = 0;i<num_outputs;i++)
    {
        BittreeNode* base_task_type_output_child =
                base_task_type->io->add_output_child(internal_node);

        for(int j = 0;j<init_output_sizes[i];j++)
        {
            base_task_type_output_child->add_child(leaf_node, new_machine_bit);
        }

        BittreeNode* init_delta_task_type_output_child =
                init_delta_task_type->add_output_child(leaf_node);

        init_delta_task_type_output_child->set_delta(
                internal_node, delta_output_size[i], new_blanko_bit);

        assert(init_delta_task_type->output->children[i]->delta != nullptr);

        BittreeNode* delta_task_type_output_child =
                delta_task_type->add_output_child(leaf_node);

        delta_task_type_output_child->set_delta(
                internal_node, delta_output_size[i], new_blanko_bit);

        assert(delta_task_type->output->children[i]->delta != nullptr);

    }
}

void IncrementalTypeExpression::build_type__add_leafs(int num_input_operands, int init_output_size, int delta_output_size)
{
    base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, leaf_node);
    delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), internal_node, leaf_node);

    build_input_type(num_input_operands);



    int num_output_operands = 1;
    int init_output_sizes[1] = {init_output_size};
    int delta_output_sizes[1] = {delta_output_size};

//    build_output_type(num_output_operands, init_output_sizes, delta_output_sizes);

    //output type expression
    for(int i = 0;i<init_output_size;i++)
    {
        BittreeNode* base_task_type__output_child =
                base_task_type->io->add_output_child(leaf_node, new_machine_bit);

//        base_task_type->output->children.push_back(
//                new BittreeNode(base_task_type->output, Name("children", base_task_type->output->children.size()),
//                        leaf_node, new_machine_bit));
    }

    init_delta_task_type->output->set_delta(internal_node, 0, new_blanko_bit);
    delta_task_type->output->set_delta(internal_node, delta_output_size, new_blanko_bit);

//    delta_task_type->output->delta = new BittreeNode(delta_task_type->output, Name("delta"), internal_node);
//    for(int i = 0;i<delta_output_size;i++)
//    {
//        delta_task_type->output->delta->children.push_back(
//                new BittreeNode(delta_task_type->output->delta,
//                        Name("children", delta_task_type->output->delta->children.size()), leaf_node, new_blanko_bit));
//    }
}

void IncrementalTypeExpression::build_input_type(int num_input_operands) {
    int init_input_sizes[num_input_operands];
    int delta_input_sizes[num_input_operands];
    for (int i = 0; i < num_input_operands; i++) {
        init_input_sizes[i] = 0;
        delta_input_sizes[i] = 1;
    }
    build_input_type(num_input_operands, init_input_sizes, delta_input_sizes);
}

void IncrementalTypeExpression::generate_deltas() {
    assert(deltas.size() == 0);
    deltas.push_back(init_delta_task_type);
    for(int i = 0;i<10;i++)
    {
        deltas.push_back(delta_task_type);
    }
}

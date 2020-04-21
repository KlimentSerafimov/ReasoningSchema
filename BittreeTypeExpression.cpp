//
// Created by Kliment Serafimov on 1/21/20.
//

#include "BittreeTypeExpression.h"
#include <iostream>
#include <vector>

using namespace std;

BittreeTypeExpression::BittreeTypeExpression(TaskName task_name)
{
    bool enter = false;
    if(task_name.do__sum)
    {
        assert(!enter);
        enter = true;
        int num_input_operands = 2;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
        int init_output_size = 1;
        int delta_output_size = 1;

        build_type(num_input_operands, init_output_size, delta_output_size);
    }

    if(task_name.do__greater)
    {

        assert(!enter);
        enter = true;
        int num_input_operands = 2;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
        int init_output_size = 1;
        int delta_output_size = 0;

        build_type(num_input_operands, init_output_size, delta_output_size);

    }

    if(task_name.do__cummulative_binary_operator)
    {
        assert(!enter);
        enter = true;
        int num_input_operands = 1;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
        int init_output_size = 1;
        int delta_output_size = 0;

        build_type(num_input_operands, init_output_size, delta_output_size);
    }

    if(task_name.do__bitwise_binary_operator)
    {
        assert(!enter);
        enter = true;
        int num_input_operands = 2;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
        int init_output_size = 0;
        int delta_output_size = 1;

        build_type(num_input_operands, init_output_size, delta_output_size);
    }

    if(task_name.do__multiply_by)
    {
        assert(!enter);
        enter = true;
        int num_input_operands = 1;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
        int init_output_size = -1;
        int delta_output_size = 1;

        if(task_name.param__multiply_by <= 1) init_output_size = 0;
        else if(task_name.param__multiply_by <= 2) init_output_size = 1;
        else if(task_name.param__multiply_by <= 4) init_output_size = 2;
        else if(task_name.param__multiply_by <= 8) init_output_size = 3;
        else if(task_name.param__multiply_by <= 16) init_output_size = 4;
        else assert(false);

        assert(init_output_size != -1);

        build_type(num_input_operands, init_output_size, delta_output_size);
    }


    if(task_name.do__add)
    {
        assert(!enter);
        enter = true;
        int num_input_operands = 1;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
        int init_output_size = -1;
        int delta_output_size = 1;

        if(task_name.param__multiply_by <= 1) init_output_size = 1;
        else if(task_name.param__multiply_by <= 2) init_output_size = 2;
        else if(task_name.param__multiply_by <= 4) init_output_size = 3;
        else if(task_name.param__multiply_by <= 8) init_output_size = 4;
        else if(task_name.param__multiply_by <= 16) init_output_size = 5;
        else assert(false);

        assert(init_output_size != -1);

        build_type(num_input_operands, init_output_size, delta_output_size);
    }

    if(task_name.do__one_shift_idx || task_name.do__count_unary)
    {
        assert(!enter);
        enter = true;
        int num_input_operands = 1;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
//        int init_output_size = 1;
//        BittreeTypeLeafNodeType delta_output_size = double_node;

        base_task_type = new BittreeTaskType(
                NULL,  Name("base_task_type"), internal_node, internal_node);
        delta_task_type = new BittreeInputOutputType(
                NULL,  Name("delta_task_type"), internal_node, leaf_node);

        build_input_type(num_input_operands);

        BittreeNode* base_task_type__output_child =
                base_task_type->io->add_output_child(leaf_node, new_machine_bit);

//        base_task_type->output->children.push_back(
//                new BittreeNode(base_task_type->output, Name("children", base_task_type->output->children.size()), leaf_node, new_machine_bit));

        BittreeNode* delta_task_type__output_delta =
                delta_task_type->output->set_delta(leaf_node, double_node);

//        delta_task_type->output->delta =
//                new BittreeNode(delta_task_type->output, Name("delta"), leaf_node, double_node);


    }

    if(task_name.do__one_shift_idx__reverse_subtask || task_name.do__count_unary__reverse_subtask)
    {

        assert(!enter);
        enter = true;
        int num_input_operands = 1;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
        int init_output_size = (1<<task_name.param__init_size);
        int delta_output_size = 0;

        build_type(num_input_operands, init_output_size, delta_output_size);

    }

    if(task_name.do__unary_sum)
    {

        assert(!enter);
        enter = true;
        int num_input_operands = 2;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
//        int init_output_size = 2;
//        BittreeTypeLeafNodeType delta_output_size = double_node;

        base_task_type =
                new BittreeTaskType(
                        NULL,  Name("base_task_type"), internal_node, internal_node);
        delta_task_type =
                new BittreeInputOutputType(
                        NULL,  Name("delta_task_type"), internal_node, leaf_node);

        build_input_type(num_input_operands);

        BittreeNode* base_task_type__output_child_1 =
                base_task_type->io->add_output_child(leaf_node, new_machine_bit);

        BittreeNode* base_task_type__output_child_2 =
                base_task_type->io->add_output_child(leaf_node, new_machine_bit);

        BittreeNode* delta_task_type__output_delta =
                delta_task_type->output->set_delta(leaf_node, double_node);

    }

    if(task_name.do__least_set_bit)
    {

        assert(!enter);
        enter = true;
        int num_input_operands = 1;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

        int num_outputs = 2;
        int init_output_sizes[2] = {1, 0};
        int delta_output_sizes[2] = {0, 1};

        base_task_type = new BittreeTaskType(NULL,  Name("base_task_type"), internal_node, internal_node);
        delta_task_type = new BittreeInputOutputType(NULL,  Name("delta_task_type"), internal_node, internal_node);

        build_input_type(num_input_operands);

        build_output_type(num_outputs, init_output_sizes, delta_output_sizes);

    }

    if(task_name.do__max_window_between_bits || task_name.do__sort_bits)
    {

        assert(!enter);
        enter = true;
        int num_input_operands = 1;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

//        int num_output = 1;
        int init_output_size = 0;
        int delta_output_size = 1;

        build_type(num_input_operands, init_output_size, delta_output_size);
    }

    if(task_name.do__max_window_between_bits_with_state)
    {

        assert(!enter);
        enter = true;
        int num_input_operands = 1;
//        int init_input_operand_sizes[1] = {0};
//        int delta_input_operand_sizes[1] = {1};

        int num_outputs = 2;
        int init_output_sizes[2] = {0, 0};
        int delta_output_sizes[2] = {1, 1};

        base_task_type = new BittreeTaskType(
                NULL,  Name("base_task_type"), internal_node, internal_node);
        delta_task_type = new BittreeInputOutputType(
                NULL,  Name("delta_task_type"), internal_node, internal_node);

        build_input_type(num_input_operands);

        build_output_type(num_outputs, init_output_sizes, delta_output_sizes);
    }

    if(task_name.do__linear_and_or_expression)
    {
        assert(!enter);
        enter = true;

        int num_input_operands = 2;
//        int init_input_operands[2] = {0, 0};
//        int delta_input_operands[2] = {1, 1};

        base_task_type = new BittreeTaskType(
                NULL,  Name("base_task_type"), internal_node, internal_node);
        delta_task_type = new BittreeInputOutputType(
                NULL,  Name("delta_task_type"), internal_node, internal_node);

        build_input_type(num_input_operands);//, init_input_operands, delta_input_operands);

        int num_outputs = 1;
        int init_output_sizes[] = {1};
        int delta_output_sizes[] = {0};

        build_output_type(num_outputs, init_output_sizes, delta_output_sizes);

//        build_type(2, 1, 0);

    }

    if(task_name.do__linear_and_or_nand_nor_expression)
    {
        assert(!enter);
        enter = true;

        int num_input_operands = 3;
        base_task_type = new BittreeTaskType(
                NULL,  Name("base_task_type"), internal_node, internal_node);
        delta_task_type = new BittreeInputOutputType(
                NULL,  Name("delta_task_type"), internal_node, internal_node);

        build_input_type(num_input_operands);//, init_input_operands, delta_input_operands);

        int num_outputs = 1;
        int init_output_sizes[] = {1};
        int delta_output_sizes[] = {0};

        build_output_type(num_outputs, init_output_sizes, delta_output_sizes);

    }

    assert(enter);
//    cout << base_task_type->to_string() << endl;
}

//void BittreeTypeExpression::build_input_type(int num_input_operands, const int init_input_operands[], int delta_input_operands[])
//{
//    //input type expression
//    for(int i = 0;i<num_input_operands;i++)
//    {
//        BittreeNode* base_task_type__input_child =
//                base_task_type->io->add_input_child(internal_node);
//
//        //init_input_operand_sizes == init_input_operands[i]
//        for(int j = 0; j< init_input_operands[i];j++)
//        {
//            BittreeNode* base_task_type__input_child__delta =
//                    base_task_type__input_child->add_child(leaf_node, shared_machine_bit);
//        }
//
//        BittreeNode* delta_task_type__input_child =
//                delta_task_type->add_input_child(leaf_node);
//
//        //delta_input_operand_sizes == delta_input_operands[i]
//        BittreeNode* delta_task_type__input_child__delta =
//                delta_task_type__input_child->set_delta(internal_node, delta_input_operands[i], shared_blanko_bit);
//    }
//}

void BittreeTypeExpression::build_input_type(int num_input_operands)
{
    //input type expression
    for(int i = 0;i<num_input_operands;i++)
    {
        //init_input_operand_sizes == 0
        BittreeNode* base_task_type__input_child =
                base_task_type->io->add_input_child(internal_node);

        BittreeNode* delta_task_type__input_child =
                delta_task_type->add_input_child(leaf_node);

        //delta_input_operand_sizes == 1
        BittreeNode* delta_task_type__input_child__delta =
                delta_task_type__input_child->set_delta(internal_node, 1, shared_blanko_bit);
    }
}

void BittreeTypeExpression::build_output_type(int num_outputs, int* init_output_sizes, int* delta_output_size)
{
    for(int i = 0;i<num_outputs;i++)
    {
        BittreeNode* base_task_type__output_child =
                base_task_type->io->add_output_child(internal_node);

        for(int j = 0;j<init_output_sizes[i];j++)
        {
            base_task_type__output_child->add_child(leaf_node, new_machine_bit);
        }

        BittreeNode* delta_task_type__output_child =
                delta_task_type->add_output_child(leaf_node);
//        delta_task_type->output->children.push_back(
//                new BittreeNode(
//                        delta_task_type->output,
//                        Name("children", delta_task_type->output->children.size()), leaf_node));

        BittreeNode* delta_task_type__output_child__delta =
                delta_task_type__output_child->set_delta(
                        internal_node, delta_output_size[i], new_blanko_bit);

        assert(delta_task_type->output->children[i]->delta != NULL);

//        delta_task_type->output->children.at(i)->delta = new BittreeNode(delta_task_type->output->children.at(i),
//                                                                             Name("delta"), internal_node);
//        for(int j = 0;j<delta_output_size[i];j++)
//        {
//            delta_task_type->output->children.at(i)->delta->children.push_back(
//                    new BittreeNode(delta_task_type->output->children.at(i)->delta,
//                            Name(
//                                    "children", delta_task_type->output->children.at(i)->delta->children.size()),
//                                    leaf_node, new_blanko_bit));
//        }
    }
}

void BittreeTypeExpression::build_type(int num_input_operands, int init_output_size, int delta_output_size)
{
    base_task_type = new BittreeTaskType(
            NULL,  Name("base_task_type"), internal_node, internal_node);
    delta_task_type = new BittreeInputOutputType(
            NULL,  Name("delta_task_type"), internal_node, leaf_node);

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

    delta_task_type->output->set_delta(internal_node, delta_output_size, new_blanko_bit);

//    delta_task_type->output->delta = new BittreeNode(delta_task_type->output, Name("delta"), internal_node);
//    for(int i = 0;i<delta_output_size;i++)
//    {
//        delta_task_type->output->delta->children.push_back(
//                new BittreeNode(delta_task_type->output->delta,
//                        Name("children", delta_task_type->output->delta->children.size()), leaf_node, new_blanko_bit));
//    }
}

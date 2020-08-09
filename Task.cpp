//
// Created by Kliment Serafimov on 1/13/20.
//

#include "Task.h"
#include "BittreeTypeExpression.h"

void Task::generate_bittree_task_expression(BittreeTypeExpression* holder)
{
    assert(false);
}

void Task::solve(BittreeInputOutputType * holder) {
    assert(false);
}

void Task__sum::generate_bittree_task_expression(BittreeTypeExpression * ret) {

//    BittreeTypeExpression ret = BittreeTypeExpression();

    int num_input_operands = 2;
    int init_output_size = 1;
    int delta_output_size = 1;

    ret->build_type(num_input_operands, init_output_size, delta_output_size);

//    return ret;
}

void Task__sum::solve(BittreeInputOutputType * holder) {

    const int num_operands = 2;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0, 0};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<holder->input->children.at(i)->children.size();j++)
        {
            assert(holder->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*holder->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }
    int sum = operands[0] + operands[1];

    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

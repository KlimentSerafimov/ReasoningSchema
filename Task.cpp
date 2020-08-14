//
// Created by Kliment Serafimov on 1/13/20.
//

#include "Task.h"
#include "IncrementalTypeExpression.h"

void Task::generate_bittree_task_expression(IncrementalTypeExpression* holder)
{
    assert(false);
}

void Task::solve(BittreeInputOutputType * holder) {
    assert(false);
}

void Task__sum::generate_bittree_task_expression(IncrementalTypeExpression * ret) {

    int num_input_operands = 2;
    int init_output_size = 1;
    int delta_output_size = 1;

    ret->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
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

void Task__greater::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 2;
    int init_output_size = 1;
    int delta_output_size = 0;
    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__greater::solve(BittreeInputOutputType *holder) {

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
    int is_0th_greater_than_1st = operands[0] > operands[1];
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(is_0th_greater_than_1st, i);
    }
}

void Task__cumulative_binary_operator::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 1;
    int init_output_size = 1;
    int delta_output_size = 0;
    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__cumulative_binary_operator::solve(BittreeInputOutputType *holder) {
    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {1};
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        for(int j = 0;j<holder->input->children.at(i)->children.size();j++)
        {
            assert(holder->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]&=holder->input->children.at(i)->children.at(j)->bit->bit_val;
        }
    }
    int culumative_binary_operator = operands[0];
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(culumative_binary_operator, i);
    }
}

void Task__bitwise_binary_operator::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 2;
    int init_output_size = 0;
    int delta_output_size = 1;
    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__bitwise_binary_operator::solve(BittreeInputOutputType *holder) {
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
    int sum = operands[0] ^ operands[1];
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__one_shift_idx::generate_bittree_task_expression(IncrementalTypeExpression *holder) {

    int num_input_operands = 1;

    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, leaf_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), internal_node, leaf_node);

    holder->build_input_type(num_input_operands);

    holder->base_task_type->io->add_output_child(leaf_node, new_machine_bit);

    holder->init_delta_task_type->output->set_delta(internal_node, 0, new_machine_bit);

    holder->delta_task_type->output->set_delta(leaf_node, double_node);

}

void Task__one_shift_idx::solve(BittreeInputOutputType *holder) {
    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    sum.set(operands[0]);
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__multiply_by::generate_bittree_task_expression(IncrementalTypeExpression *holder)
{
    int num_input_operands = 1;
    int init_output_size = -1;
    int delta_output_size = 1;

    if(multiply_by <= 1) init_output_size = 0;
    else if(multiply_by <= 2) init_output_size = 1;
    else if(multiply_by <= 4) init_output_size = 2;
    else if(multiply_by <= 8) init_output_size = 3;
    else if(multiply_by <= 16) init_output_size = 4;
    else if(multiply_by <= 32) init_output_size = 5;
    else if(multiply_by <= 64) init_output_size = 6;
    else if(multiply_by <= 128) init_output_size = 6;
    else assert(false);

    assert(init_output_size != -1);

    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__multiply_by::solve(BittreeInputOutputType *holder) {
    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
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
    int sum = operands[0] * multiply_by;
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__one_shift_idx__rev::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 1;
    int init_output_size = (1<<param__init_size);
    int delta_output_size = 0;

    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__one_shift_idx__rev::solve(BittreeInputOutputType *holder) {
    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    sum.set(operands[0] << (param__init_size - holder->input->children.at(0)->children.size()));
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__count_unary::generate_bittree_task_expression(IncrementalTypeExpression *holder) {

    int num_input_operands = 1;

    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, leaf_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), internal_node, leaf_node);

    holder->build_input_type(num_input_operands);

    BittreeNode* base_task_type__output_child =
            holder->base_task_type->io->add_output_child(leaf_node, new_machine_bit);

    holder->init_delta_task_type->output->set_delta(internal_node, 0, new_machine_bit);

    BittreeNode* delta_task_type__output_delta =
            holder->delta_task_type->output->set_delta(leaf_node, double_node);

}

void Task__count_unary::solve(BittreeInputOutputType *holder) {
    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    if(operands[0]>=1) {
        sum.set_range(0, operands[0] - 1);
    }
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__count_unary__rev::generate_bittree_task_expression(IncrementalTypeExpression *holder) {

    int num_input_operands = 1;
    int init_output_size = (1<<param__init_size);
    int delta_output_size = 0;

    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__count_unary__rev::solve(BittreeInputOutputType *holder) {

    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    sum.set_range(0, (operands[0] << (param__init_size - holder->input->children.at(0)->children.size())));
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__unary_sum::generate_bittree_task_expression(IncrementalTypeExpression *holder) {

    int num_input_operands = 2;

    holder->base_task_type =
            new BittreeTaskType(
                    nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type =
            new BittreeInputOutputType(
                    nullptr,  Name("init_delta_task_type"), internal_node, leaf_node);
    holder->delta_task_type =
            new BittreeInputOutputType(
                    nullptr,  Name("delta_task_type"), internal_node, leaf_node);

    holder-> build_input_type(num_input_operands);

    holder->base_task_type->io->add_output_child(leaf_node, new_machine_bit);

    holder->base_task_type->io->add_output_child(leaf_node, new_machine_bit);

    holder->init_delta_task_type->output->set_delta(internal_node, 0, new_blanko_bit);

    holder->delta_task_type->output->set_delta(leaf_node, double_node);

}

void Task__unary_sum::solve(BittreeInputOutputType *holder) {
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
    Bitvector sum = 0;
    assert(operands[0]+operands[1] < holder->output->children.size());
    sum.set(operands[0]+operands[1]);
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__least_set_bit::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 1;
    int num_outputs = 2;
    int init_output_sizes[2] = {1, 0};
    int delta_output_sizes[2] = {0, 1};

    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, internal_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), internal_node, internal_node);

    holder->build_input_type(num_input_operands);

    holder->build_output_type(num_outputs, init_output_sizes, delta_output_sizes);
}

void Task__least_set_bit::solve(BittreeInputOutputType *holder) {

    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
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
    Bitvector sum = 0;
    int edge_case_bit = 0;
    if(operands[0] == 0)
    {
        edge_case_bit = 1;
    }
    else
    {
        sum.set(num_trailing_zeroes(operands[0]));
    }

    assert(holder->output->children.at(0)->children.at(0)->bit->is_bit_set == false);
    holder->output->children.at(0)->children.at(0)->bit->is_bit_set = true;
    holder->output->children.at(0)->children.at(0)->bit->bit_val = edge_case_bit;

    for(int i = 0;i<holder->output->children.at(1)->children.size();i++)
    {
        assert(holder->output->children.at(1)->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(1)->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(1)->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__strech_of_0s::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 1;
    int init_output_size = 0;
    int delta_output_size = 1;

    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__strech_of_0s::solve(BittreeInputOutputType *holder) {

    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    int len_operands[num_operands];
    for(int i = 0;i<num_operands;i++)
    {
        int pow = 1;
        len_operands[i] = holder->input->children.at(i)->children.size();
        for(int j = 0;j<holder->input->children.at(i)->children.size();j++)
        {
            assert(holder->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i]+=pow*holder->input->children.at(i)->children.at(j)->bit->bit_val;
            pow*=2;
        }
    }

    int prev_set_bit = -1;
    pair<int, pair<int, int> > max_window = make_pair(-1, make_pair(-1, -1));
    for(int i = 0; i<holder->input->children.at(0)->children.size();i++)
    {
        if(get_bit(Bitvector(operands[0], len_operands[0]) , i))
        {
            if(prev_set_bit == -1)
            {
                prev_set_bit = i;
            }
            else
            {
                max_window = max(make_pair(i-prev_set_bit+1, make_pair(prev_set_bit, i)), max_window);
                prev_set_bit = i;
            }
        }
    }

    Bitvector sum = 0;

    if(max_window.first != -1)
    {
        sum.set_range(0, max_window.second.second-max_window.second.first+1-1);
    }

    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }

}

void Task__strech_of_0s_w_state::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 1;
    int num_outputs = 2;
    int init_output_sizes[2] = {0, 0};
    int delta_output_sizes[2] = {1, 1};

    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, internal_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), internal_node, internal_node);

    holder->build_input_type(num_input_operands);

    holder->build_output_type(num_outputs, init_output_sizes, delta_output_sizes);
}

void Task__strech_of_0s_w_state::solve(BittreeInputOutputType *holder) {

    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
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

    int prev_set_bit = -1;
    pair<int, pair<int, int> > max_window = make_pair(-1, make_pair(-1, -1));
    int len = holder->input->children.at(0)->children.size();
    for(int i = 0; i<len;i++)
    {
        if(get_bit(operands[0], i))
        {
            if(prev_set_bit == -1)
            {
                prev_set_bit = i;
            }
            else
            {
                max_window = max(make_pair(i-prev_set_bit+1, make_pair(prev_set_bit, i)), max_window);
                prev_set_bit = i;
            }
        }
    }

    Bitvector sum_result = 0;
    Bitvector sum_counting = 0;

    if(max_window.first != -1)
    {
        sum_result.set_range(0, max_window.second.second-max_window.second.first+1-1);
    }
    if(prev_set_bit != -1)
    {
        sum_counting.set_range(0, len-1-prev_set_bit+1-1);
    }

    for(int i = 0;i<holder->output->children.at(0)->children.size();i++)
    {
        assert(holder->output->children.at(0)->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(0)->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(0)->children.at(i)->bit->bit_val = get_bit(sum_result, i);
    }
    for(int i = 0;i<holder->output->children.at(1)->children.size();i++)
    {
        assert(holder->output->children.at(1)->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(1)->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(1)->children.at(i)->bit->bit_val = get_bit(sum_counting, i);
    }
}

void Task__linear_and_or_expression::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 2;
    int init_input_operands[2] = {1, 0};
    int delta_input_operands[2] = {1, 1};

    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, internal_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), internal_node, internal_node);

    holder->build_input_type(num_input_operands, init_input_operands, delta_input_operands);//, init_input_operands, delta_input_operands);

    int num_outputs = 1;
    int init_output_sizes[] = {1};
    int delta_output_sizes[] = {0};

    holder->build_output_type(num_outputs, init_output_sizes, delta_output_sizes);

}

void Task__linear_and_or_expression::solve(BittreeInputOutputType *holder) {

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

    int rez = get_bit(operands[0], 0);
    int len = holder->input->children.at(0)->children.size();
    for(int i = 1;i<len;i++)
    {
        if(get_bit(operands[1], i-1))
        {
            rez &= get_bit(operands[0], i);
        } else
        {
            rez |= get_bit(operands[0], i);
        }
    }

    for(int i = 0;i<holder->output->children.at(0)->children.size();i++)
    {
        assert(holder->output->children.at(0)->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(0)->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(0)->children.at(i)->bit->bit_val = get_bit(rez, i);
    }
}

void Task__linear_and_or_nand_nor_expression::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 3;
    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, internal_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), internal_node, internal_node);

    holder->build_input_type(num_input_operands);//, init_input_operands, delta_input_operands);

    int num_outputs = 1;
    int init_output_sizes[] = {1};
    int delta_output_sizes[] = {0};

    holder->build_output_type(num_outputs, init_output_sizes, delta_output_sizes);
}

void Task__linear_and_or_nand_nor_expression::solve(BittreeInputOutputType *holder) {

    const int num_operands = 3;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0, 0, 0};
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

    int rez = get_bit(operands[0], 0);
    int len = holder->input->children.at(0)->children.size();
    for(int i = 1;i<len;i++)
    {
        if(get_bit(operands[2], i-1)) {
            if (get_bit(operands[1], i - 1)) {
                rez &= get_bit(operands[0], i);
            } else {
                rez |= get_bit(operands[0], i);
            }
        }
        else {
            if (get_bit(operands[1], i - 1)) {
                rez = !get_bit((rez & get_bit(operands[0], i)), 0);
            } else {
                rez = !get_bit((rez | get_bit(operands[0], i)), 0);
            }
        }
    }

    for(int i = 0;i<holder->output->children.at(0)->children.size();i++)
    {
        assert(holder->output->children.at(0)->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(0)->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(0)->children.at(i)->bit->bit_val = get_bit(rez, i);
    }
}

void Task__sort_bits::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 1;
    int init_output_size = 0;
    int delta_output_size = 1;

    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__sort_bits::solve(BittreeInputOutputType *holder) {
    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    int len_operands[num_operands];
    for(int i = 0;i<num_operands;i++) {
        int pow = 1;
        len_operands[i] = holder->input->children.at(i)->children.size();
        for (int j = 0; j < holder->input->children.at(i)->children.size(); j++) {
            assert(holder->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i] += pow * holder->input->children.at(i)->children.at(j)->bit->bit_val;
            pow *= 2;
        }
    }

    Bitvector sorted_bits = 0;

    int num_ones = __builtin_popcount(operands[0]);

    if(num_ones >= 1)
    {
        sorted_bits.set_range(len_operands[0]-1-num_ones+1, len_operands[0]-1);
    }

    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sorted_bits, i);
    }
}

void Task__add::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    int num_input_operands = 1;
    int init_output_size = -1;
    int delta_output_size = 1;

    if(add_num <= 1) init_output_size = 1;
    else if(add_num <= 2) init_output_size = 2;
    else if(add_num <= 4) init_output_size = 3;
    else if(add_num <= 8) init_output_size = 4;
    else if(add_num <= 16) init_output_size = 5;
    else if(add_num <= 32) init_output_size = 6;
    else assert(false);

    assert(init_output_size != -1);

    holder->build_type__add_leafs(num_input_operands, init_output_size, delta_output_size);
}

void Task__add::solve(BittreeInputOutputType *holder) {

    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
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
    int sum = operands[0] + add_num;
    for(int i = 0;i<holder->output->children.size();i++)
    {
        assert(holder->output->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(i)->bit->bit_val = get_bit(sum, i);
    }
}

void Task__gene_network::generate_bittree_task_expression(IncrementalTypeExpression *holder) {

    int num_input_operands = 1;
    int init_input_operand_sizes[1] = {0};
    int delta_input_operand_sizes[1] = {2};

    //input:
    //
    //__
    //____
    //..

    int num_outputs = 3;
    int init_output_sizes[3] = {1, 0, 0};
    int delta_output_sizes[3] = {0, 1, 1};

    holder->base_task_type =
            new BittreeTaskType(nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, internal_node);
    holder->delta_task_type =
            new BittreeInputOutputType(nullptr,  Name("delta_task_type"), internal_node, internal_node);

    holder->build_input_type(num_input_operands, init_input_operand_sizes, delta_input_operand_sizes);

    holder->build_output_type(num_outputs, init_output_sizes, delta_output_sizes);

}

void Task__gene_network::solve(BittreeInputOutputType *holder) {
    const int num_operands = 1;
    assert(holder->input->children.size() == num_operands);
    int operands[num_operands] = {0};
    int len_operands[num_operands];
    for(int i = 0;i<num_operands;i++) {
        int pow = 1;
        len_operands[i] = holder->input->children.at(i)->children.size();
        for (int j = 0; j < holder->input->children.at(i)->children.size(); j++) {
            assert(holder->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            operands[i] += pow * holder->input->children.at(i)->children.at(j)->bit->bit_val;
            pow *= 2;
        }
    }

    Bitvector final = 0;
    Bitvector bits = 0;
    Bitvector intermediate = 0;

    if(len_operands[0] >= 1)
    {
        assert(len_operands[0]%2==0);
        vector<int> genes;
        for(int i = 0;i<len_operands[0]/2;i++)
        {
            int bit0 = get_bit(operands[0], 2*i);
            int bit1 = get_bit(operands[0], 2*i+1);
            genes.push_back(bit0 + 2*bit1);
        }

        vector<bool> a_or_i;

        for(int i = 0; i<genes.size()-1; i++)
        {
            int op = genes[i+1];
            int operand = genes[i];
            int bit = get_bit(param__network, operand+4*op);
            bits.set(i, bit);
            a_or_i.push_back(bit);
        }

        int bool_running_a_or_i = 1;

        for(int i = a_or_i.size()-1;i>=0;i--)
        {
            if(bool_running_a_or_i)
            {
                if(a_or_i[i])
                {
                    bool_running_a_or_i = 1;
                } else{
                    bool_running_a_or_i = 0;
                }
            }
            else
            {
                if(a_or_i[i])
                {
                    bool_running_a_or_i = 1;
                } else{
                    bool_running_a_or_i = 1;
                }
            }
            intermediate.set(i, bool_running_a_or_i);
        }

        final = bool_running_a_or_i;

    }

    for(int i = 0;i<holder->output->children.at(0)->children.size();i++)
    {
        assert(holder->output->children.at(0)->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(0)->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(0)->children.at(i)->bit->bit_val = get_bit(final, i);
    }
    for(int i = 0;i<holder->output->children.at(1)->children.size();i++)
    {
        assert(holder->output->children.at(1)->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(1)->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(1)->children.at(i)->bit->bit_val = get_bit(bits, i);
    }
    for(int i = 0;i<holder->output->children.at(2)->children.size();i++)
    {
        assert(holder->output->children.at(2)->children.at(i)->bit->is_bit_set == false);
        holder->output->children.at(2)->children.at(i)->bit->is_bit_set = true;
        holder->output->children.at(2)->children.at(i)->bit->bit_val = get_bit(intermediate, i);
    }
}

void grid_in_grid_out(int in_w, int out_w, IncrementalTypeExpression *holder)
{
    int init_input_operands[8] = {1, 1, 1, 1, 1, 1, 1, 1};
    int delta_input_operands[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    int num_input_operands = in_w;
    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), internal_node, internal_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), internal_node, internal_node);

    holder->build_input_type(num_input_operands, init_input_operands, delta_input_operands);

    int num_outputs = out_w;
    const int max_w = 8;
    assert(num_outputs <= max_w);
    int init_output_sizes[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    int delta_output_sizes[8] = {1, 1, 1, 1, 1, 1, 1, 1};

    holder->build_output_type(num_outputs, init_output_sizes, delta_output_sizes);
}

void list_in_sum_in_unary_out(IncrementalTypeExpression *holder, int num_bits)
{
    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), leaf_node, leaf_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), leaf_node, leaf_node);

    BittreeNode* input_leaf_delta = new BittreeNode(nullptr, Name("input_leaf_delta"), internal_node);
    BittreeNode* integer_node = input_leaf_delta->push_back_child(new BittreeNode(input_leaf_delta, Name("children", 0), internal_node));
    for(int i = 0;i<num_bits;i++)
    {
        integer_node->add_child(leaf_node, shared_blanko_bit);
    }

    BittreeNode* output_leaf_delta = new BittreeNode(nullptr, Name("output_leaf_delta"), internal_node);
    for(int i = 0;i<(1<<num_bits);i++)
    {
        output_leaf_delta->add_child(leaf_node, new_blanko_bit);
    }

    holder->init_delta_task_type->input->set_delta(input_leaf_delta);
    holder->delta_task_type->input->set_delta(input_leaf_delta);

    holder->init_delta_task_type->output->set_delta(output_leaf_delta);
    holder->delta_task_type->output->set_delta(output_leaf_delta);

}

void Task__biggest_square::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    grid_in_grid_out(param__w, param__w, holder);
}

vector<vector<int> > get_input_grid(int param__w, BittreeInputOutputType *holder)
{
    const int num_operands = param__w;
    assert(holder->input->children.size() == num_operands);
    vector<vector<int> > input_grid;
    for(int i = 0;i<num_operands;i++)
    {
        input_grid.emplace_back();
        for(int j = 0;j<holder->input->children.at(i)->children.size();j++)
        {
            assert(holder->input->children.at(i)->children.at(j)->bit->is_bit_set == true);
            input_grid[i].push_back(holder->input->children.at(i)->children.at(j)->bit->bit_val);
        }
    }

    return input_grid;
}

int get_bitvector_as_int(BittreeNode* node)
{
    int ret = 0;
    int pow = 1;
    int len_operands = node->children.size();
    for (int j = 0; j < node->children.size(); j++) {
        assert(node->children.at(j)->bit->is_bit_set == true);
        ret += pow * node->children.at(j)->bit->bit_val;
        pow *= 2;
    }
    return ret;
}

vector<int> get_input_list_of_bitvectors_as_ints(BittreeInputOutputType *holder)
{
    vector<int> input_grid;
    for(int i = 0;i<holder->input->children.size();i++)
    {
        input_grid.push_back(get_bitvector_as_int(holder->input->children[i]));
    }

    return input_grid;
}

pair<int, pair<pair<int, int>, pair<int, int> > > largest_rectangle(int param__w, vector<vector<int> > input_grid)
{

    pair<int, pair<pair<int, int>, pair<int, int> > > best =
            make_pair(-1, make_pair(make_pair(0, 0), make_pair(0, 0)));

    for(int x = 0; x < input_grid.size(); x++) {
        for (int y = 0; y < input_grid[x].size(); y++) {
            for (int xx = x; xx < input_grid.size(); xx++) {
                for (int yy = y; yy < input_grid[xx].size(); yy++) {
                    int count = 0;
                    int sum = 0;
                    for (int xxx = x; xxx <= xx; xxx++) {
                        for (int yyy = y; yyy <= min(yy, (int) input_grid[xxx].size() - 1); yyy++) {
                            count++;
                            sum += input_grid[xxx][yyy];
                        }
                    }
                    if (sum == count) {
                        if (sum > best.first || best.first == -1) {
                            best = make_pair(sum, make_pair(make_pair(x, y), make_pair(xx, yy)));
                        }
                    }
                }
            }
        }
    }
    return best;
}


vector<vector<int> > mark_largest_rectangle(vector<vector<int> > input_grid, pair<int, pair<pair<int, int>, pair<int, int> > > best)
{
    vector<vector<int> > output_grid;
    for(int i = 0;i<input_grid.size();i++)
    {
        output_grid.emplace_back(vector<int>());
        for(int j = 0;j<input_grid[i].size();j++)
        {
            output_grid[i].push_back(0);
        }
    }

    if(best.first >= 1) {
        int best_x = best.second.first.first;
        int best_y = best.second.first.second;
        int best_xx = best.second.second.first;
        int best_yy = best.second.second.second;

        for (int x = best_x; x <= best_xx; x++) {
            for (int y = best_y; y <= min(best_yy, (int) input_grid[x].size() - 1); y++) {
                output_grid[x][y] = 1;
            }
        }
    }
    return output_grid;
}

void project_output_grid(BittreeInputOutputType *holder, vector<vector<int> > output_grid)
{
    for(int i = 0;i<holder->output->children.size(); i++)
    {
        for(int j = 0;j<(int)holder->output->children.at(i)->children.size();j++)
        {
            assert(holder->output->children.at(i)->children.at(j)->bit->is_bit_set == false);
            holder->output->children.at(i)->children.at(j)->bit->is_bit_set = true;
            holder->output->children.at(i)->children.at(j)->bit->bit_val = output_grid[i][j];
        }
    }
}

void project_output_bitvector(BittreeInputOutputType *holder, vector<int> output_grid)
{
    for(int j = 0;j<(int)holder->output->children.size();j++)
    {
        assert(holder->output->children.at(j)->bit->is_bit_set == false);
        holder->output->children.at(j)->bit->is_bit_set = true;
        holder->output->children.at(j)->bit->bit_val = output_grid[j];
    }
}

void Task__biggest_square::solve(BittreeInputOutputType *holder) {

    vector<vector<int> > input_grid = get_input_grid(param__w, holder);

    pair<int, pair<pair<int, int>, pair<int, int> > > best =
            largest_rectangle(param__w, input_grid);

    vector<vector<int> > output_grid = mark_largest_rectangle(input_grid, best);

    project_output_grid(holder, output_grid);
}

void Task__biggest_square_with_kernel::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    grid_in_grid_out(param__w, param__w+param__w-1, holder);
}

void Task__biggest_square_with_kernel::solve(BittreeInputOutputType *holder) {
    vector<vector<int> > input_grid = get_input_grid(param__w, holder);

    pair<int, pair<pair<int, int>, pair<int, int> > > best =
            largest_rectangle(param__w, input_grid);

    vector<vector<int> > output_grid = mark_largest_rectangle(input_grid, best);

    vector<vector<int> > intermediate_state;
    for (int x = 1; x < input_grid.size(); x++) {
        intermediate_state.emplace_back(vector<int>());
        for (int y = 0; y < input_grid[x].size(); y++) {
            intermediate_state[x-1].push_back(input_grid[x][y] & input_grid[x - 1][y]);
        }
    }

    for(int i = 0;i<holder->output->children.size(); i++)
    {
        for(int j = 0;j<(int)holder->output->children.at(i)->children.size();j++)
        {
            assert(holder->output->children.at(i)->children.at(j)->bit->is_bit_set == false);
            holder->output->children.at(i)->children.at(j)->bit->is_bit_set = true;
            if(i < output_grid.size()) {
                holder->output->children.at(i)->children.at(j)->bit->bit_val = output_grid[i][j];
            }
            else
            {
                holder->output->children.at(i)->children.at(j)->bit->bit_val = intermediate_state[i - output_grid.size()][j];
            }
        }
    }
}


void Task__biggest_square_w_corners_as_intermediate_state::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    grid_in_grid_out(param__w, param__w+param__w, holder);
}

void Task__biggest_square_w_corners_as_intermediate_state::solve(BittreeInputOutputType *holder) {
    vector<vector<int> > input_grid = get_input_grid(param__w, holder);

    pair<int, pair<pair<int, int>, pair<int, int> > > best =
            largest_rectangle(param__w, input_grid);

    vector<vector<int> > output_grid = mark_largest_rectangle(input_grid, best);

    int best_x = best.second.first.first;
    int best_y = best.second.first.second;
    int best_xx = best.second.second.first;
    int best_yy = best.second.second.second;

    vector<vector<int> > intermediate_state;
    for (int x = 0; x < input_grid.size(); x++) {
        intermediate_state.emplace_back(vector<int>());
        for (int y = 0; y < input_grid[x].size(); y++) {
            if(x == best_x && y == best_y)
            {
                intermediate_state[x].push_back(1);
            }
            else if(x == best_xx && y == best_yy)
            {
                intermediate_state[x].push_back(1);
            }
            else
            {
                intermediate_state[x].push_back(0);
            }
        }
    }

    for(int i = 0;i<holder->output->children.size(); i++)
    {
        for(int j = 0;j<(int)holder->output->children.at(i)->children.size();j++)
        {
            assert(holder->output->children.at(i)->children.at(j)->bit->is_bit_set == false);
            holder->output->children.at(i)->children.at(j)->bit->is_bit_set = true;
            if(i < output_grid.size()) {
                holder->output->children.at(i)->children.at(j)->bit->bit_val = output_grid[i][j];
            }
            else
            {
                holder->output->children.at(i)->children.at(j)->bit->bit_val = intermediate_state[i - output_grid.size()][j];
            }
        }
    }
}

void Task__biggest_square_as_corners::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    grid_in_grid_out(param__w, param__w, holder);
}

void Task__biggest_square_as_corners::solve(BittreeInputOutputType *holder) {
    vector<vector<int> > input_grid = get_input_grid(param__w, holder);

    pair<int, pair<pair<int, int>, pair<int, int> > > best =
            largest_rectangle(param__w, input_grid);

    int best_x = best.second.first.first;
    int best_y = best.second.first.second;
    int best_xx = best.second.second.first;
    int best_yy = best.second.second.second;

    vector<vector<int> > output_grid;
    for (int x = 0; x < input_grid.size(); x++) {
        output_grid.emplace_back(vector<int>());
        for (int y = 0; y < input_grid[x].size(); y++) {
            if(x == best_x && y == best_y)
            {
                output_grid[x].push_back(1);
            }
            else if(x == best_xx && y == best_yy)
            {
                output_grid[x].push_back(1);
            }
            else
            {
                output_grid[x].push_back(0);
            }
        }
    }

    project_output_grid(holder, output_grid);
}

void Task__remove_points::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    grid_in_grid_out(param__w, param__w, holder);
}

int sum_four_neighbours(vector<vector<int> > input_grid, int x, int y)
{
    const int num_neighbours = 4;
    int delta[num_neighbours][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
    int sum = 0;
    for(int i = 0;i<num_neighbours;i++)
    {
        int next_x = x+delta[i][0];
        int next_y = y+delta[i][1];
        if(next_x >= 0 && next_x < input_grid.size() && next_y>=0 && next_y<input_grid[next_x].size())
        {
            sum += input_grid[next_x][next_y];
        }
    }
    return sum;
}

int sum_eight_neighbours(vector<vector<int> > input_grid, int x, int y)
{
    const int num_neighbours = 8;
    int delta[num_neighbours][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}, {-1, -1}, {1, -1}, {-1, 1}, {1, 1}};
    int sum = 0;
    for(int i = 0;i<num_neighbours;i++)
    {
        int next_x = x+delta[i][0];
        int next_y = y+delta[i][1];
        if(next_x >= 0 && next_x < input_grid.size() && next_y>=0 && next_y<input_grid[next_x].size())
        {
            sum += input_grid[next_x][next_y];
        }
    }
    return sum;
}

void Task__remove_points::solve(BittreeInputOutputType *holder) {
    vector<vector<int> > input_grid = get_input_grid(param__w, holder);

    vector<vector<int> > output_grid;
    for(int i = 0;i<input_grid.size();i++)
    {
        output_grid.emplace_back(vector<int>());
        for(int j = 0;j<input_grid[i].size(); j++)
        {
            if(input_grid[i][j] == 1 && sum_four_neighbours(input_grid, i, j) == 0)
            {
                output_grid[i].push_back(0);
            }
            else
            {
                output_grid[i].push_back(input_grid[i][j]);
            }
        }
    }

    project_output_grid(holder, output_grid);
}

void Task__remove_points_and_peninsula::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    grid_in_grid_out(param__w, 2*param__w, holder);
}

void Task__remove_points_and_peninsula::solve(BittreeInputOutputType *holder) {
    vector<vector<int> > input_grid = get_input_grid(param__w, holder);

    vector<vector<int> > output_grid;
    for(int i = 0;i<input_grid.size();i++)
    {
        output_grid.emplace_back(vector<int>());
        for(int j = 0;j<input_grid[i].size(); j++)
        {
            if(input_grid[i][j] == 1 && sum_four_neighbours(input_grid, i, j) == 0)
            {
                output_grid[i].push_back(0);
            }
            else
            {
                output_grid[i].push_back(input_grid[i][j]);
            }
        }
    }

    vector<vector<int> > second_output;
    for(int i = 0;i<input_grid.size();i++)
    {
        second_output.emplace_back(vector<int>());
        for(int j = 0;j<input_grid[i].size(); j++)
        {
            if(input_grid[i][j] == 1 && sum_four_neighbours(input_grid, i, j) == 1)
            {
                second_output[i].push_back(0);
            }
            else
            {
                second_output[i].push_back(input_grid[i][j]);
            }
        }
    }

    for(int i = 0;i<holder->output->children.size(); i++)
    {
        for(int j = 0;j<(int)holder->output->children.at(i)->children.size();j++)
        {
            assert(holder->output->children.at(i)->children.at(j)->bit->is_bit_set == false);
            holder->output->children.at(i)->children.at(j)->bit->is_bit_set = true;
            if(i < output_grid.size()) {
                holder->output->children.at(i)->children.at(j)->bit->bit_val = output_grid[i][j];
            }
            else
            {
                holder->output->children.at(i)->children.at(j)->bit->bit_val = second_output[i - output_grid.size()][j];
            }
        }
    }
}



void Task__game_of_life::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    grid_in_grid_out(param__w, param__w, holder);
}

vector<vector<int> > produce_new_grid_with_same_shape_and_all_0s(vector<vector<int> > input_grid)
{
    vector<vector<int> > output_grid;
    for(int i = 0;i<input_grid.size();i++)
    {
        output_grid.emplace_back(vector<int>());
        for(int j = 0;j<input_grid[i].size(); j++)
        {
            output_grid[i].push_back(0);
        }
    }

    return output_grid;
}

//incremental task := {task_0, task_1, task_2 .. }
//task_0 := {instace_0, instance_1 ... }
//...

void Task__game_of_life::solve(BittreeInputOutputType *holder) {
    vector<vector<int> > input_grid = get_input_grid(param__w, holder);
    vector<vector<int> > output_grid = produce_new_grid_with_same_shape_and_all_0s(input_grid);
//    For a space that is 'populated':
//    Each cell with one or no neighbors dies, as if by solitude.
//            Each cell with four or more neighbors dies, as if by overpopulation.
//            Each cell with two or three neighbors survives.
//    For a space that is 'empty' or 'unpopulated'
//            Each cell with three neighbors becomes populated.

    for(int i = 0;i<input_grid.size();i++)
    {
        for(int j = 0;j<input_grid[i].size();j++)
        {
            if(input_grid[i][j] == 1)
            {
                if(sum_eight_neighbours(input_grid, i, j) <= 1)
                {

                } else if(sum_eight_neighbours(input_grid, i, j) >= 4)
                {

                }
                else
                {
                    output_grid[i][j] = 1;
                }
            }
            else
            {
                if(sum_eight_neighbours(input_grid, i, j) == 3) {
                    output_grid[i][j] = 1;
                }
            }
        }
    }

    project_output_grid(holder, output_grid);
}

void Task__sum_of_n_k_bit_integers_in_unary::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    list_in_sum_in_unary_out(holder, param_k);
}

void Task__sum_of_n_k_bit_integers_in_unary::solve(BittreeInputOutputType *holder) {
    vector<int> input_list = get_input_list_of_bitvectors_as_ints(holder);
    int sum = 0;
    for(int i = 0;i<input_list.size();i++)
    {
        sum += input_list[i];
    }
    vector<int> output_bitvector;
    for(int i = 0;i<holder->output->children.size();i++)
    {
        output_bitvector.push_back(i<sum);
    }
    project_output_bitvector(holder, output_bitvector);
}

void Task__sum_of_n_k_bit_integers::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), leaf_node, leaf_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), leaf_node, leaf_node);

    BittreeNode* input_leaf_delta = new BittreeNode(nullptr, Name("input_leaf_delta"), internal_node);
    BittreeNode* integer_node = input_leaf_delta->push_back_child(new BittreeNode(input_leaf_delta, Name("children", 0), internal_node));
    for(int i = 0;i<param_k;i++)
    {
        integer_node->add_child(leaf_node, shared_blanko_bit);
    }

    BittreeNode* init_output_leaf_delta = new BittreeNode(nullptr, Name("output_leaf_delta"), internal_node);
    for(int i = 0;i<param_k;i++)
    {
        init_output_leaf_delta->add_child(leaf_node, new_blanko_bit);
    }

    holder->init_delta_task_type->input->set_delta(input_leaf_delta);
    holder->init_delta_task_type->output->set_delta(init_output_leaf_delta);

    BittreeNode* output_leaf_delta = new BittreeNode(nullptr, Name("output_leaf_delta"), internal_node);
    output_leaf_delta->add_child(leaf_node, new_blanko_bit);

    holder->delta_task_type->input->set_delta(input_leaf_delta);
    holder->delta_task_type->output->set_delta(output_leaf_delta);
}

void Task__sum_of_n_k_bit_integers::solve(BittreeInputOutputType *holder) {
    vector<int> input_list = get_input_list_of_bitvectors_as_ints(holder);
    int sum = 0;
    for(int i = 0;i<input_list.size();i++)
    {
        sum += input_list[i];
    }
    vector<int> output_bitvector;
    for(int i = 0;i<holder->output->children.size();i++)
    {
        output_bitvector.push_back(get_bit(sum, i));
    }
    project_output_bitvector(holder, output_bitvector);
}


void Task__sum_of_n_k_bit_integers_with_intermediate_state::generate_bittree_task_expression(IncrementalTypeExpression *holder) {
    holder->base_task_type = new BittreeTaskType(
            nullptr,  Name("base_task_type"), internal_node, internal_node);
    holder->init_delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("init_delta_task_type"), leaf_node, leaf_node);
    holder->delta_task_type = new BittreeInputOutputType(
            nullptr,  Name("delta_task_type"), leaf_node, leaf_node);

    BittreeNode* input_leaf_delta = new BittreeNode(nullptr, Name("input_leaf_delta"), internal_node);
    BittreeNode* integer_node = input_leaf_delta->push_back_child(new BittreeNode(input_leaf_delta, Name("children", 0), internal_node));
    for(int i = 0;i<param_k;i++)
    {
        integer_node->add_child(leaf_node, shared_blanko_bit);
    }

    BittreeNode* init_output_leaf_delta = new BittreeNode(nullptr, Name("output_leaf_delta"), internal_node);
    for(int i = 0;i<param_k;i++)
    {
        init_output_leaf_delta->add_child(leaf_node, new_blanko_bit);
    }

    holder->init_delta_task_type->input->set_delta(input_leaf_delta);
    holder->init_delta_task_type->output->set_delta(init_output_leaf_delta);

    BittreeNode* output_leaf_delta = new BittreeNode(nullptr, Name("output_leaf_delta"), internal_node);
    output_leaf_delta->add_child(leaf_node, new_blanko_bit);

    holder->delta_task_type->input->set_delta(input_leaf_delta);
    holder->delta_task_type->output->set_delta(output_leaf_delta);
}

void Task__sum_of_n_k_bit_integers_with_intermediate_state::solve(BittreeInputOutputType *holder) {
    vector<int> input_list = get_input_list_of_bitvectors_as_ints(holder);
    int sum = 0;
    for(int i = 0;i<input_list.size();i++)
    {
        sum += input_list[i];
    }
    vector<int> output_bitvector;
    for(int i = 0;i<holder->output->children.size();i++)
    {
        output_bitvector.push_back(get_bit(sum, i));
    }
    project_output_bitvector(holder, output_bitvector);
}

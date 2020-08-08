//
// Created by Kliment Serafimov on 1/13/20.
//

#ifndef COMPACTPOSET_TASKNAME_H
#define COMPACTPOSET_TASKNAME_H

#include <string>
#include <assert.h>

using namespace std;

static const string str_task_name__sum = "sum";
static const string str_task_name__greater = "greater";
static const string str_task_name__cumulative_binary_operator = "cumulative_binary_operator";
static const string str_task_name__bitwise_binary_operator = "bitwise_binary_operator";
static const string str_task_name__one_shift_idx = "one_shift_idx";
static const string str_task_name__multiply_by = "multiply_by";
static const string str_task_name__one_shift_idx__reverse_subtask = "one_shift_idx__rev";
static const string str_task_name__count_unary = "count_unary";
static const string str_task_name__count_unary__reverse_subtask = "count_unary__rev";
static const string str_task_name__unary_sum = "unary_sum";
static const string str_task_name__least_set_bit = "least_set_bit";
static const string str_task_name__max_window_between_bits = "strech_of_0s";
static const string str_task_name__max_window_between_bits_with_state = "strech_of_0s_w_state";
static const string str_task_name__linear_and_or_expression = "linear_and_or_expression";
static const string str_task_name__linear_and_or_nand_nor_expression = "linear_and_or_nand_nor_expression";
static const string str_task_name__sort_bits = "sort_bits";
static const string str_task_name__add = "add";
static const string str_task_name__gene_network = "gene_network";
static const string str_task_name__biggest_square = "biggest_square";
static const string str_task_name__biggest_square_with_kernel = "biggest_square_with_kernel";

enum TaskNames {
    sum,
    greater,
    cumulative_binary_operator,
    bitwise_binary_operator,
    one_shift_idx,
    multiply_by,
    one_shift_idx__rev,
    count_unary,
    count_unary__rev,
    unary_sum,
    least_set_bit,
    strech_of_0s,
    strech_of_0s_w_state,
    linear_and_or_expression,
    linear_and_or_nand_nor_expression,
    sort_bits,
    add,
    gene_network,
    biggest_square,
    biggest_square_with_kernel
    };

class TaskName
{
public:

    string task_name;

    bool do__sum = false;
    bool do__greater = false;
    bool do__cummulative_binary_operator = false;
    bool do__bitwise_binary_operator = false;
    bool do__one_shift_idx = false;
    bool do__count_unary = false;
    bool do__unary_sum = false;
    bool do__least_set_bit = false;
    bool do__max_window_between_bits = false;
    bool do__max_window_between_bits_with_state = false;
    bool do__linear_and_or_expression = false;
    bool do__linear_and_or_nand_nor_expression = false;
    bool do__sort_bits = false;

    bool do__multiply_by = false;
    int param__multiply_by = -1;

    bool do__add = false;
    int param__add_by = -1;

    bool do__one_shift_idx__reverse_subtask = false;
    bool do__count_unary__reverse_subtask = false;

    int param__init_size = -1;

    bool do__gene_network = false;
    int param__network = -1;

    bool do__biggest_square = false;
    int param__w = -1;

    bool do__biggest_square_with_kernel = false;

    bool num_iter_defined = false;
    int num_iter = -1;

    TaskName() = default;

    TaskName(string _task_name) {

        assert(_task_name != str_task_name__multiply_by);
        assert(_task_name != str_task_name__one_shift_idx__reverse_subtask);
        assert(_task_name != str_task_name__count_unary__reverse_subtask);
        assert(_task_name != str_task_name__add);
        assert(_task_name != str_task_name__gene_network);
        assert(_task_name != str_task_name__biggest_square);
        assert(_task_name != str_task_name__biggest_square_with_kernel);

        task_name = _task_name;

        do__sum = task_name == str_task_name__sum;
        do__greater = task_name == str_task_name__greater;
        do__cummulative_binary_operator = task_name == str_task_name__cumulative_binary_operator;
        do__bitwise_binary_operator = task_name == str_task_name__bitwise_binary_operator;
        do__one_shift_idx = task_name == str_task_name__one_shift_idx;
        do__count_unary = task_name == str_task_name__count_unary;
        do__unary_sum = task_name == str_task_name__unary_sum;
        do__least_set_bit = task_name == str_task_name__least_set_bit;
        do__max_window_between_bits = task_name == str_task_name__max_window_between_bits;
        do__max_window_between_bits_with_state = task_name == str_task_name__max_window_between_bits_with_state;
        do__linear_and_or_expression = task_name == str_task_name__linear_and_or_expression;
        do__linear_and_or_nand_nor_expression = task_name == str_task_name__linear_and_or_nand_nor_expression;
        do__sort_bits = task_name == str_task_name__sort_bits;
    }

    TaskName(string _task_name, int _param) {
        assert(
                _task_name == str_task_name__multiply_by ||
                _task_name == str_task_name__one_shift_idx__reverse_subtask ||
                _task_name == str_task_name__count_unary__reverse_subtask ||
                _task_name == str_task_name__add ||
                _task_name == str_task_name__gene_network ||
                _task_name == str_task_name__biggest_square
        );
        task_name = _task_name;

        do__multiply_by = task_name == str_task_name__multiply_by;
        do__one_shift_idx__reverse_subtask = task_name == str_task_name__one_shift_idx__reverse_subtask;
        do__count_unary__reverse_subtask = task_name == str_task_name__count_unary__reverse_subtask;
        do__add = task_name == str_task_name__add;
        do__gene_network = task_name == str_task_name__gene_network;
        do__biggest_square = task_name == str_task_name__biggest_square;

        if(do__multiply_by)
        {
            param__multiply_by = _param;
        }
        else if(do__one_shift_idx__reverse_subtask || do__count_unary__reverse_subtask)
        {
            param__init_size = _param;
            num_iter_defined = true;
            num_iter = param__init_size;
        }
        else if(do__add)
        {
            param__add_by = _param;
        }
        else if(do__gene_network)
        {
            param__network = _param;
        }
        else if(do__biggest_square)
        {
            param__w = _param;
        }
        else
        {
            assert(false);
        }
    }


    TaskName(string _task_name, int param0, int param1, int param2) {
        assert(_task_name == str_task_name__biggest_square_with_kernel);
    }

    string get_task_name()
    {
        if(do__multiply_by)
        {
            return task_name + "(num=" + std::to_string(param__multiply_by) + ")";
        }
        else if(do__one_shift_idx__reverse_subtask || do__count_unary__reverse_subtask)
        {
            return task_name + "(init_size=" + std::to_string(param__init_size) + ")";
        }
        else if(do__add)
        {
            return task_name + "(num=" + std::to_string(param__add_by) + ")";
        }
        else if(do__gene_network)
        {
            return task_name + "(network=" + std::to_string(param__network) + ")";
        }
        else{
            return task_name;
        }
    }
};

class Task_Sum : public TaskName
{
    Task_Sum();
};

#endif //COMPACTPOSET_TASKNAME_H

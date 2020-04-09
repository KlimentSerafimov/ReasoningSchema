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
static const string str_task_name__one_shift_idx__reverse_subtask = "one_shift_idx__reverse_subtask";
static const string str_task_name__count_unary = "count_unary";
static const string str_task_name__count_unary__reverse_subtask = "count_unary__reverse_subtask";
static const string str_task_name__unary_sum = "unary_sum";
static const string str_task_name__least_set_bit = "least_set_bit";
static const string str_task_name__max_window_between_bits = "max_window_between_bits";

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

    bool do__multiply_by = false;
    int param__multiply_by = -1;

    bool do__one_shift_idx__reverse_subtask = false;
    bool do__count_unary__reverse_subtask = false;

    int param__init_size = -1;

    bool num_iter_defined = false;
    int num_iter = -1;

    TaskName() = default;

    TaskName(string _task_name) {

        assert(_task_name != str_task_name__multiply_by);
        assert(_task_name != str_task_name__one_shift_idx__reverse_subtask);
        assert(_task_name != str_task_name__count_unary__reverse_subtask);

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
    }

    TaskName(string _task_name, int _param) {
        assert(
                _task_name == str_task_name__multiply_by ||
                _task_name == str_task_name__one_shift_idx__reverse_subtask ||
                _task_name == str_task_name__count_unary__reverse_subtask
        );
        task_name = _task_name;

        do__multiply_by = task_name == str_task_name__multiply_by;
        do__one_shift_idx__reverse_subtask = task_name == str_task_name__one_shift_idx__reverse_subtask;
        do__count_unary__reverse_subtask = task_name == str_task_name__count_unary__reverse_subtask;

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
        else
        {
            assert(false);
        }
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
        else{
            return task_name;
        }
    }
};



#endif //COMPACTPOSET_TASKNAME_H

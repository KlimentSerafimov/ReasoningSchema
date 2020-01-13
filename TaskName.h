//
// Created by Kliment Serafimov on 1/13/20.
//

#ifndef COMPACTPOSET_TASKNAME_H
#define COMPACTPOSET_TASKNAME_H

#include <string>

using namespace std;

class TaskName
{
public:
    string task_name;

    bool do_sum_task_type = false;
    bool do_greater_task_type = false;
    bool do_cummulative_binary_operator = false;
    bool do_bitwise_binary_operator = false;
    bool do_multiplication_by = false;
    int multiply_by = false;

    TaskName() = default;

    TaskName(string _task_name) {

        assert(_task_name != "multiply_by");
        task_name = _task_name;

        do_sum_task_type = task_name == "sum";
        do_greater_task_type = task_name == "greater";
        do_cummulative_binary_operator = task_name == "cumulative_binary_operator";
        do_bitwise_binary_operator = task_name == "bitwise_binary_operator";
    }

    TaskName(string _task_name, int _multiply_by) {
        assert(_task_name == "multiply_by");
        task_name = _task_name;
        multiply_by = _multiply_by;

        do_multiplication_by = task_name == "multiply_by";
    }

    string get_task_name()
    {
        if(do_multiplication_by)
        {
            return task_name+"(num="+std::to_string(multiply_by)+")";
        } else{
            return task_name;
        }
    }
};



#endif //COMPACTPOSET_TASKNAME_H

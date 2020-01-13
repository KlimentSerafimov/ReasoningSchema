//
// Created by Kliment Serafimov on 2019-11-18.
//

#ifndef COMPACTPOSET_PARTIALFUNCTION_H
#define COMPACTPOSET_PARTIALFUNCTION_H

#include <string>
#include <vector>
#include "BitInBittree.h"
#include "BittreeTaskType.h"

using namespace std;


static string set_operation_name[3] = {"difference", "intersection", "my_union"};

enum SetOperationType {difference, intersection, my_union};


class PartialFunction
{

public:
    int function_size;
    int total_function;
    unsigned int partition;

    PartialFunction();
    PartialFunction(vector<BitInBittree*> bits);
    PartialFunction(int _function_size, int _total_function, int _partition);

    void init_via_bits(vector<BitInBittree*> bits);

    string to_string();

    bool has_empty_intersection_with(PartialFunction other);

    void append_difference_with(PartialFunction other, vector<PartialFunction> &to_append_to);

    void append_intersection_with(PartialFunction other, vector<PartialFunction> &to_append_to);

    bool is_contained_in(PartialFunction other);

    PartialFunction get_composition(PartialFunction other);

    int has_output(int idx);

    int get_output(int idx);

    void apply_common_partition(PartialFunction other);

    void set_bit(int idx, int value);

    void clear_bit(int idx);

    int partition_size();

    bool empty();

    bool full();
};

class BittreeTaskTypeAsPartialFunction : public PartialFunction
{
public:
    BittreeTaskType* bittree_taks_type;
    BittreeTaskTypeAsPartialFunction(BittreeTaskType* _bittree_taks_type)
    {
        bittree_taks_type = _bittree_taks_type;

        vector<BitInBittree*> partial_bits;
        BittreeTaskType* local_subtask = bittree_taks_type;
        int num_prev_subtasks = 0;
        while(local_subtask != NULL)
        {
            if(num_prev_subtasks < 1 || local_subtask->solution == NULL)
            {
                local_subtask->append_bits(partial_bits);
                local_subtask = local_subtask->subtask;
            } else
            {
                local_subtask->solution->append_bits(partial_bits);
                local_subtask = local_subtask->subtask;
            }
            if(num_prev_subtasks == 1)
            {
                break;
            }
            num_prev_subtasks+=1;
        }
        init_via_bits(partial_bits);
    }
};

#endif //COMPACTPOSET_PARTIALFUNCTION_H

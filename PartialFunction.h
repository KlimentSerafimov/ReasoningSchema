//
// Created by Kliment Serafimov on 2019-11-18.
//

#ifndef COMPACTPOSET_PARTIALFUNCTION_H
#define COMPACTPOSET_PARTIALFUNCTION_H

#include <string>
#include <vector>
#include "BitInBittree.h"
#include "Bitvector.h"

class BittreeTaskType;

using namespace std;


static string set_operation_name[3] = {"difference", "intersection", "my_union"};

enum SetOperationType {difference, intersection, my_union};


class PartialFunction
{

public:
    int function_size;
    Bitvector total_function;
    Bitvector partition;


    vector<BitInBittree*> bits;

    PartialFunction();
    PartialFunction(vector<BitInBittree*> bits);
    PartialFunction(int _function_size, Bitvector _total_function);
    PartialFunction(int _function_size, Bitvector _total_function, Bitvector _partition);

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

    bool operator < (const PartialFunction& other) const;

    bool operator == (const PartialFunction& other) const;

    string bit_to_string(int i);
};

class BittreeTaskTypeAsPartialFunction : public PartialFunction
{
public:
    BittreeTaskType* bittree_task_type{};
    int subtask_depth{};
    BittreeTaskTypeAsPartialFunction(BittreeTaskType *_bittree_taks_type, int subtask_depth);

    BittreeTaskTypeAsPartialFunction(const PartialFunction& partial_function);

    BittreeTaskTypeAsPartialFunction(vector<BitInBittree*> _bits, BittreeTaskType *_bittree_taks_type, int subtask_depth);

    BittreeTaskTypeAsPartialFunction() = default;

    string to_string__one_line();
};

#endif //COMPACTPOSET_PARTIALFUNCTION_H

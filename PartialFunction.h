//
// Created by Kliment Serafimov on 2019-11-18.
//

#ifndef COMPACTPOSET_PARTIALFUNCTION_H
#define COMPACTPOSET_PARTIALFUNCTION_H

#include <string>
#include <vector>

using namespace std;


static string set_operation_name[3] = {"difference", "intersection", "my_union"};

enum SetOperationType {difference, intersection, my_union};


class PartialFunction
{

public:
    int num_inputs;
    int function_size;
    int total_function;
    unsigned int partition;

    PartialFunction();
    PartialFunction(int _num_inputs, int _total_function, int _partition);

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
};

#endif //COMPACTPOSET_PARTIALFUNCTION_H

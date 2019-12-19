//
// Created by Kliment Serafimov on 2019-11-18.
//

#ifndef COMPACTPOSET_PARTIALFUNCTION_H
#define COMPACTPOSET_PARTIALFUNCTION_H

#include <string>

using namespace std;

class PartialFunction
{

public:
    int num_inputs;
    int function_size;
    int total_function;
    int partition;

    PartialFunction();
    PartialFunction(int _num_inputs, int _total_function, int _partition);

    string to_string();

    bool is_generalization_of(PartialFunction other);

    PartialFunction get_composition(PartialFunction other);

    int has_output(int idx);

    int get_output(int idx);

    void apply_intersection(PartialFunction apply_intersection);

    void set_bit(int idx, int value);

    void clear_bit(int idx);

    int partition_size();

    bool empty();
};

#endif //COMPACTPOSET_PARTIALFUNCTION_H

//
// Created by Kliment Serafimov on 2019-11-18.
//

#ifndef COMPACTPOSET_METAEXAMPLE_H
#define COMPACTPOSET_METAEXAMPLE_H

#include "PartialFunction.h"
#include <string>

using namespace std;

class MetaExample
{
public:
    PartialFunction partial_function;
    PartialFunction generalization;

    MetaExample() = default;

    MetaExample(int _num_inputs, int _total_function, int _partition);
    MetaExample(int _num_inputs, int _total_function, int _partition, int generalization_partition);
    MetaExample(PartialFunction partial_function);

    int get_compact_partial_outputs();
    int get_compact_hidden_outputs();

    string linear_string(int tab);

    string to_string();

    void print();

    bool operator < (const MetaExample &other) const
    {
        return generalization.total_function < other.generalization.total_function;
    }
};


#endif //COMPACTPOSET_METAEXAMPLE_H

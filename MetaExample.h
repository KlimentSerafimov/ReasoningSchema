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
    PartialFunction total_function;

    MetaExample() = default;

    MetaExample(int _num_inputs, int _total_function, int _partition);
    MetaExample(PartialFunction partial_function);

    int get_compact_partial_outputs();
    int get_compact_hidden_outputs();

    string linear_string(int tab);

    string linear_string();

    void print();
};


#endif //COMPACTPOSET_METAEXAMPLE_H

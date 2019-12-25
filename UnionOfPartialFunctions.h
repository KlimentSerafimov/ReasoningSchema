//
// Created by Kliment Serafimov on 2019-12-25.
//

#ifndef COMPACTPOSET_UNIONOFPARTIALFUNCTION_H
#define COMPACTPOSET_UNIONOFPARTIALFUNCTION_H

#include "PartialFunction.h"
#include <vector>

using namespace std;

class UnionOfPartialFunctions {

    void init(PartialFunction partial_function);

public:
    int num_inputs;

    vector<PartialFunction> partial_functions;

    UnionOfPartialFunctions() = default;

    UnionOfPartialFunctions(int _num_inputs);

    UnionOfPartialFunctions(UnionOfPartialFunctions *to_point_to);

    UnionOfPartialFunctions(PartialFunction partial_function);

    UnionOfPartialFunctions(PartialFunction partial_function, SetOperationType operation_type, UnionOfPartialFunctions *other);

    UnionOfPartialFunctions copy();

    void my_delete();

    void apply_operation(SetOperationType operation_type, UnionOfPartialFunctions *other);

    bool is_empty();

    string to_string();

    string to_string(int num_tabs);

    vector<PartialFunction> get_union_of_partial_functions();

    void append_union_of_partial_functions_that_contain_partial_function(
            PartialFunction partial_function, vector<PartialFunction>* ret);

    string get_string_of_union_of_partial_functions(int num_tabs);

    bool contains(PartialFunction partial_function);
};


#endif //COMPACTPOSET_UNIONOFPARTIALFUNCTION_H

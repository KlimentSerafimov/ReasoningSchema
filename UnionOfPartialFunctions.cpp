//
// Created by Kliment Serafimov on 2019-12-25.
//

#include "UnionOfPartialFunctions.h"
#include <iostream>


UnionOfPartialFunctions::UnionOfPartialFunctions(int _num_inputs) {
    num_inputs = _num_inputs;
}

UnionOfPartialFunctions::UnionOfPartialFunctions(UnionOfPartialFunctions *to_point_to) {
    num_inputs = to_point_to->num_inputs;
    partial_functions = to_point_to->partial_functions;
}

UnionOfPartialFunctions::UnionOfPartialFunctions(PartialFunction partial_function) {
    init(partial_function);
}


UnionOfPartialFunctions::UnionOfPartialFunctions(PartialFunction partial_function, SetOperationType operation_type,
                                                 UnionOfPartialFunctions *other) {
    init(partial_function);
    apply_operation(operation_type, other);
}

UnionOfPartialFunctions UnionOfPartialFunctions::copy() {
    return UnionOfPartialFunctions(this);
}

void UnionOfPartialFunctions::my_delete() {
    partial_functions.clear();
}

void UnionOfPartialFunctions::init(PartialFunction partial_function) {
    num_inputs = partial_function.num_inputs;
    partial_functions.push_back(partial_function);
}

bool UnionOfPartialFunctions::is_empty() {
    return partial_functions.size() == 0;
}

vector<PartialFunction> UnionOfPartialFunctions::get_union_of_partial_functions() {
    return partial_functions;
}

bool UnionOfPartialFunctions::contains(PartialFunction partial_function) {
    for(int i = 0;i<partial_functions.size();i++)
    {
//        if(partial_functions[i].is_contained_in(partial_function) || partial_function.is_contained_in(partial_functions[i]))
        if(!partial_functions[i].has_empty_intersection_with(partial_function))
        {
            return true;
        }
    }
    return false;
}

void UnionOfPartialFunctions::append_union_of_partial_functions_that_contain_partial_function(
        PartialFunction partial_function, vector<PartialFunction> *ret) {
    for (int i = 0; i < partial_functions.size(); i++) {
//        if (partial_functions[i].is_contained_in(partial_function) || partial_function.is_contained_in(partial_functions[i]))
        if(!partial_functions[i].has_empty_intersection_with(partial_function))
        {
            ret->push_back(partial_functions[i].get_composition(partial_function));
        }
    }
}

void UnionOfPartialFunctions::apply_operation(SetOperationType operation_type, UnionOfPartialFunctions *other)
{
    if(operation_type == difference)
    {
        vector<PartialFunction> ret;
        for(int i = 0;i<partial_functions.size();i++)
        {
            vector<PartialFunction> intermediate_results;
            intermediate_results.push_back(partial_functions[i]);
            for(int j = 0;j<other->partial_functions.size();j++)
            {
                if (!partial_functions[i].has_empty_intersection_with(other->partial_functions[j]))
                {
                    vector<PartialFunction> local_intermediate_results;
                    assert(intermediate_results.size() != 0);
                    for(int k = 0; k<intermediate_results.size();k++) {
                        intermediate_results[k].append_difference_with(
                                other->partial_functions[j], local_intermediate_results);
                    }
//                    assert(local_intermediate_results.size() != 0);
                    intermediate_results = local_intermediate_results;
                }
            }
            for(int j = 0;j<intermediate_results.size(); j++)
            {
                ret.push_back(intermediate_results[j]);
            }
        }
        partial_functions = ret;
    }
    else if(operation_type == intersection)
    {
        vector<PartialFunction> ret;
        for(int i = 0;i<partial_functions.size();i++)
        {
            for(int j = 0;j<other->partial_functions.size();j++)
            {
                partial_functions[i].append_intersection_with(other->partial_functions[j], ret);
            }
        }
        partial_functions = ret;
    }
    else if(operation_type == my_union)
    {
        apply_operation(difference, other);
        for(int i = 0;i<other->partial_functions.size();i++)
        {
            partial_functions.push_back(other->partial_functions[i]);
        }
    }
}

string UnionOfPartialFunctions::to_string() {
    string ret;
    for(int i = 0;i<partial_functions.size();i++)
    {
        ret+=partial_functions[i].to_string()+"\n";
    }
    return ret;
}


string UnionOfPartialFunctions::get_string_of_union_of_partial_functions(int num_tabs) {
    string tabs = "";
    for(int i = 0;i<num_tabs;i++)
    {
        tabs+="\t";
    }
    string ret;
    for(int i = 0;i<partial_functions.size();i++)
    {
        ret+=tabs+partial_functions[i].to_string() + "\n";
    }
    return ret;
}

//

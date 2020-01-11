//
// Created by Kliment Serafimov on 1/8/20.
//

#include "BitvectorTasks.h"

#include <iostream>

using namespace std;

void BitvectorTasks::populate_ordering_over_boolean_functions_with_small_sum()
{
    assert((function_size-1)%3 == 0);
    int operator_size = function_size/3;
    vector<pair<int, int> > operators;
    vector<pair<pair<int, int>, int> > operators_ids_by_heuristic;
    int first_operand_size = operator_size;
    int second_operand_size = operator_size;
    for(int i = 0;i<(1<<first_operand_size); i++)
    {
        for(int j = 0;j<(1<<second_operand_size); j++)
        {
            operators.push_back(make_pair(i,  j));
            int k = i+j;
            operators_ids_by_heuristic.push_back(make_pair(make_pair(k, max(i, j)), operators.size()-1));
        }
    }
    sort(operators_ids_by_heuristic.begin(), operators_ids_by_heuristic.end());
    for(int i = 0;i<operators_ids_by_heuristic.size();i++)
    {
        int id = operators_ids_by_heuristic[i].second;
        int total_function =
                operators[id].first + (operators[id].second << first_operand_size) + ((operators[id].first + operators[id].second) << (first_operand_size+second_operand_size));
        PartialFunction partial_function = PartialFunction(function_size, total_function, -1);
        ordering_over_boolean_functions.push_back(partial_function);
        cout << partial_function.to_string() << endl;
    }
}


void BitvectorTasks::populate_ordering_over_boolean_functions_with_bitwise_and()
{
    assert(function_size%3 == 0);
    vector<pair<int, int> > operators;
    vector<pair<pair<int, int>, int> > operators_ids_by_heuristic;
    int first_operand_size = 5;
    int second_operand_size = 5;
    for(int i = 0;i<(1<<first_operand_size); i++)
    {
        for(int j = 0;j<(1<<second_operand_size); j++)
        {
            operators.push_back(make_pair(i,  j));
            int k = i&j;
            operators_ids_by_heuristic.push_back(make_pair(make_pair(k, max(i, j)), operators.size()-1));
        }
    }
    sort(operators_ids_by_heuristic.begin(), operators_ids_by_heuristic.end());
    for(int i = 0;i<operators_ids_by_heuristic.size();i++)
    {
        int id = operators_ids_by_heuristic[i].second;
        int total_function =
                operators[id].first + (operators[id].second << first_operand_size) + ((operators[id].first&operators[id].second) << (first_operand_size+second_operand_size));
        PartialFunction partial_function = PartialFunction(function_size, total_function, (1<<16)-1-(1<<15));
        ordering_over_boolean_functions.push_back(partial_function);
        cout << partial_function.to_string() << endl;
    }
}

MetaExample BitvectorTasks::get_meta_example(PartialFunction partial_function) {
    for(int i = 0;i<ordering_over_boolean_functions.size();i++)
    {
        PartialFunction boolean_function = ordering_over_boolean_functions[i];
//        is_generalization_counter += __builtin_popcount(partial_function.partition);
        if(boolean_function.is_contained_in(partial_function))
        {
            return MetaExample(boolean_function.function_size, boolean_function.total_function, partial_function.partition);
        }
    }
    return MetaExample(partial_function.function_size, partial_function.total_function, partial_function.partition, partial_function.partition);
}

BitvectorTasks::BitvectorTasks(int _function_size, int task_id) {
    function_size = _function_size;
    if(task_id == 0)
    {
        populate_ordering_over_boolean_functions_with_small_sum();
    }
    if(task_id == 1)
    {
        populate_ordering_over_boolean_functions_with_bitwise_and();
    }
}

DeltaBittreeType::DeltaBittreeType(NodeType delta_node_type)
{
    node_type = leaf_node;
    delta = new BittreeTypeNode(delta_node_type);
}

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
    int idx;
    bool active = true;

    MetaExample() = default;

    MetaExample(int _num_inputs, int _total_function, int _partition);
    MetaExample(int _num_inputs, int _total_function, int _partition, int generalization_partition);
    MetaExample(int _num_inputs, int _total_function, int _partition, int generalization_partition, int _idx);
    MetaExample(int _num_inputs, int _total_function, int _partition, int generalization_partition, int _idx, bool _active);
    MetaExample(PartialFunction partial_function);

    int get_compact_partial_outputs();
    int get_compact_hidden_outputs();

    string linear_string(int tab);

    MetaExample get_application_of_subdomain(int subdomain_mask)
    {
        return MetaExample(
                partial_function.num_inputs,
                generalization.total_function & subdomain_mask,
                partial_function.partition & subdomain_mask,
                generalization.partition & subdomain_mask,
                idx,
                active
                );
    }

    bool fully_constrained()
    {
        return (partial_function.partition & generalization.partition) == generalization.partition;
    }

    string to_string();

    void print();

    bool operator < (const MetaExample &other) const
    {
        return generalization.total_function < other.generalization.total_function;
    }

    bool is_consistent_with(MetaExample other);
};


#endif //COMPACTPOSET_METAEXAMPLE_H

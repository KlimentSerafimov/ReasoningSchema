//
// Created by Kliment Serafimov on 2019-12-18.
//

#ifndef COMPACTPOSET_MINIMALFACTORINGSCHEMA_H
#define COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

#include <vector>
#include "CompactPoset.h"
#include "MetaExample.h"


class MinimalFactoringSchema
{
    int num_inputs;
    int function_size;
    vector<MetaExample> meta_examples;

    vector<int> get_meta_example_ids_that_are_individually_consistent_with_all_other_meta_examples_in_subdomain(
            int subdomain_mask, CompactPoset *local_compact_poset);
public:
    MinimalFactoringSchema(int _num_inputs, vector<MetaExample> _meta_examples);
};



#endif //COMPACTPOSET_MINIMALFACTORINGSCHEMA_H

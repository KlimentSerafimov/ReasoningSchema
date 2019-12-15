//
// Created by Kliment Serafimov on 2019-12-10.
//

#ifndef COMPACTPOSET_REASONINGSCHEMA_H
#define COMPACTPOSET_REASONINGSCHEMA_H


#include "CompactPoset.h"

enum ReasoningSchemaType {leaf_schema, factorization_schema, continuation_schema};

class ReasoningSchema
{
    ReasoningSchemaType schema_type;

    // if schema_type == leaf_schema
    CompactPoset compact_poset;

    // if schema_type == factorization_schema || schema_type == continuation_schema
    ReasoningSchema* root_schema;

    // if schema_type == factorization_schema
    pair<ReasoningSchema*, ReasoningSchema*> factor_schemas;

    // if schema_type == continuation_schema
    ReasoningSchema* continuation_schema;

    int num_inputs;
    int function_size;

    int min_num_necessary_meta_examples;


public:
    ReasoningSchema(int num_inputs, int generalization_mask, int training_mask, vector<MetaExample> meta_examples);

    ReasoningSchema(int num_inputs, vector<MetaExample> train_meta_examples, vector<pair<int, int> > masks, vector<pair<int, int> > training_masks);

    bool test(vector<MetaExample> test_meta_examples);

    vector<PartialFunction> query(PartialFunction partial_function);

    int get_num_necessary_meta_examples();

};


#endif //COMPACTPOSET_REASONINGSCHEMA_H

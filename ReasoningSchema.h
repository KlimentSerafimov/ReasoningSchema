//
// Created by Kliment Serafimov on 2019-12-10.
//

#ifndef COMPACTPOSET_REASONINGSCHEMA_H
#define COMPACTPOSET_REASONINGSCHEMA_H


#include "CompactPoset.h"

enum ReasoningSchemaType {leaf_schema, factorization_schema};

class ReasoningSchema
{
    ReasoningSchemaType schema_type;

    // if schema_type == leaf_schema
    CompactPoset compact_poset;

    // if schema_type == factorization_schema
    ReasoningSchema* root_schema;
    ReasoningSchema* factor_schemas;

    ReasoningSchema(int num_inputs, vector<MetaExample> meta_examples, vector<int> masks)
    {

    }
};


#endif //COMPACTPOSET_REASONINGSCHEMA_H

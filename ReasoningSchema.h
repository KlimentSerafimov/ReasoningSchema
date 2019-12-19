//
// Created by Kliment Serafimov on 2019-12-10.
//

#ifndef COMPACTPOSET_REASONINGSCHEMA_H
#define COMPACTPOSET_REASONINGSCHEMA_H


#include "CompactPoset.h"


class SwitchMasks
{
public:
    int generalization_mask;
    int optional_training_mask;
    int compulsory_training_mask;
    SwitchMasks(
            int _generalization_mask,
            int _optional_training_mask,
            int _compulsory_training_mask)

    {
        generalization_mask = _generalization_mask;
        optional_training_mask = _optional_training_mask;
        compulsory_training_mask = _compulsory_training_mask;
    }
};

class SubdomainSwitchPosets
{
    int num_inputs;
    int function_size;

public:

    int common_generalization_mask;
    int common_optional_mask;
    int all_subsets_compulsory_mask;

    map<int, int> compusorty_mask_to_id;
    vector<SwitchMasks> switch_masks;
    vector<CompactPoset> subdomain_posets;

    SubdomainSwitchPosets(int _num_inputs, int common_generalization_mask, int common_optional_mask, int all_subsets_compulsory_mask);

    string masks_to_string(int id);
};

class SubdomainSwitchPosetsFactors
{
public:
    int num_inputs;
    vector<SubdomainSwitchPosets> factors;
    vector<MetaExample> contained_meta_examples;
    vector<MetaExample> inconsistent_meta_examples;

    SubdomainSwitchPosetsFactors(int _num_inputs, pair<int, int> generalization_mask, pair<int, int> training_mask);

    void insert(MetaExample meta_example);

    void print();
};


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

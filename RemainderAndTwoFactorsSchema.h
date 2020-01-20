//
// Created by Kliment Serafimov on 2019-12-10.
//

#ifndef COMPACTPOSET_REMAINDERANDTWOFACTORSSCHEMA_H
#define COMPACTPOSET_REMAINDERANDTWOFACTORSSCHEMA_H


#include "CompactPoset.h"


class SwitchMasks
{
public:
    Bitvector generalization_mask;
    Bitvector optional_training_mask;
    Bitvector compulsory_training_mask;
    SwitchMasks(
            Bitvector _generalization_mask,
            Bitvector _optional_training_mask,
            Bitvector _compulsory_training_mask)

    {
        generalization_mask = _generalization_mask;
        optional_training_mask = _optional_training_mask;
        compulsory_training_mask = _compulsory_training_mask;
    }
};

class SubdomainSwitchPosets
{
    int function_size;

public:

    Bitvector common_generalization_mask;
    Bitvector common_optional_mask;
    Bitvector all_subsets_compulsory_mask;

    map<Bitvector, int> compusorty_mask_to_id;
    vector<SwitchMasks> switch_masks;
    vector<CompactPoset> subdomain_posets;

    SubdomainSwitchPosets(
            int _function_size, Bitvector common_generalization_mask, Bitvector common_optional_mask, Bitvector all_subsets_compulsory_mask);

    string masks_to_string(int id);
};

class SubdomainSwitchPosetsFactors
{
public:
    int function_size;
    vector<SubdomainSwitchPosets> factors;
    vector<MetaExample> contained_meta_examples;
    vector<MetaExample> inconsistent_meta_examples;

    SubdomainSwitchPosetsFactors(int _function_size, pair<Bitvector, Bitvector> generalization_mask, pair<Bitvector, Bitvector> training_mask);

    void insert(MetaExample meta_example);

    void print();
};


enum ReasoningSchemaType {leaf_schema, factorization_schema, continuation_schema};

class RemainderAndTwoFactorsSchema
{
    ReasoningSchemaType schema_type;

    // if schema_type == leaf_schema
    CompactPoset compact_poset;

    // if schema_type == factorization_schema || schema_type == continuation_schema
    RemainderAndTwoFactorsSchema* root_schema;

    // if schema_type == factorization_schema
    pair<RemainderAndTwoFactorsSchema*, RemainderAndTwoFactorsSchema*> factor_schemas;

    // if schema_type == continuation_schema
    RemainderAndTwoFactorsSchema* continuation_schema;

    int function_size;

    int min_num_necessary_meta_examples;


public:
    RemainderAndTwoFactorsSchema(int function_size, Bitvector generalization_mask, Bitvector training_mask, vector<MetaExample> meta_examples);

    RemainderAndTwoFactorsSchema(
            int function_size, vector<MetaExample> train_meta_examples,
            vector<pair<Bitvector, Bitvector> > masks, vector<pair<Bitvector, Bitvector> > training_masks);

    bool test(vector<MetaExample> test_meta_examples);

    vector<PartialFunction> query(PartialFunction partial_function);

    int get_num_necessary_meta_examples();

};


#endif //COMPACTPOSET_REMAINDERANDTWOFACTORSSCHEMA_H

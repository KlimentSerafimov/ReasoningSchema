//
// Created by Kliment Serafimov on 2019-12-10.
//

#include "ReasoningSchema.h"
#include <iostream>

using namespace std;


SubdomainSwitchPosets::SubdomainSwitchPosets(
        int _num_inputs, int _common_generalization_mask, int _common_optional_mask, int _all_subsets_compulsory_mask)
{
    common_generalization_mask = _common_generalization_mask;
    common_optional_mask = _common_optional_mask;
    all_subsets_compulsory_mask = _all_subsets_compulsory_mask;

    num_inputs = _num_inputs;
    function_size = (1 << num_inputs);

    int num_bits_in_compulsory = __builtin_popcount(all_subsets_compulsory_mask);

    for(int compulsory_mask_bits = 0;compulsory_mask_bits < (1<<num_bits_in_compulsory); compulsory_mask_bits++)
    {
        int compulsory_mask = 0;
        for(int i = 0, j = 0; i < function_size; i++)
        {
            if(get_bit(all_subsets_compulsory_mask, i))
            {
                compulsory_mask += (get_bit(compulsory_mask_bits, j)<<i);
                j++;
            }
        }

        assert(switch_masks.size() == subdomain_posets.size());
        compusorty_mask_to_id[compulsory_mask] = (int) switch_masks.size();

        switch_masks.push_back(
                SwitchMasks(common_generalization_mask, common_optional_mask, compulsory_mask));

        subdomain_posets.push_back(
                CompactPoset(num_inputs, common_generalization_mask, common_optional_mask | compulsory_mask));
    }

}

string SubdomainSwitchPosets::masks_to_string(int id)
{
    return bitvector_to_str(common_generalization_mask, function_size) + " " +
    bitvector_to_str(common_optional_mask, function_size) + " " +
    bitvector_to_str(switch_masks[id].compulsory_training_mask, function_size);
}

SubdomainSwitchPosetsFactors::SubdomainSwitchPosetsFactors(
        int _num_inputs, pair<int, int> generalization_mask, pair<int, int> training_mask)
{
    num_inputs = _num_inputs;
    factors.push_back(
            SubdomainSwitchPosets(
                    num_inputs,
                    generalization_mask.first,
                    generalization_mask.first & training_mask.first,
                    training_mask.first - (generalization_mask.first & training_mask.first)));
    factors.push_back(
            SubdomainSwitchPosets(
                    num_inputs,
                    generalization_mask.second,
                    generalization_mask.second & training_mask.second,
                    training_mask.second - (generalization_mask.second & training_mask.second)));
}

void SubdomainSwitchPosetsFactors::insert(MetaExample meta_example)
{
    PartialFunction partial_function = meta_example.partial_function;
    PartialFunction generalization = meta_example.generalization;

    assert((generalization.partition & partial_function.partition) == partial_function.partition);

    bool inserted = true;

    for(int i = 0;i<factors.size();i++)
    {
        int compulsory_training_mask = factors[i].all_subsets_compulsory_mask & partial_function.partition;
        int training_mask = compulsory_training_mask | (factors[i].common_optional_mask & partial_function.partition);
        int generalization_mask = training_mask | factors[i].common_generalization_mask;

        MetaExample subdomain_meta_example = MetaExample(
                num_inputs,
                generalization.total_function,
                training_mask, generalization_mask, meta_example.idx);

        if(generalization_mask != training_mask) {
            if (!factors[i].subdomain_posets[factors[i].compusorty_mask_to_id[compulsory_training_mask]].insert(subdomain_meta_example)) {
                for (int j = 0; j <= i; j++) {
                    int compulsory_training_mask_prev = factors[j].all_subsets_compulsory_mask & partial_function.partition;
                    int training_mask_prev = compulsory_training_mask_prev | (factors[j].common_optional_mask & partial_function.partition);
                    int generalization_mask_prev = training_mask_prev | factors[j].common_generalization_mask;

                    if(generalization_mask_prev != training_mask_prev) {
                        factors[j].subdomain_posets[factors[j].compusorty_mask_to_id[compulsory_training_mask_prev]].hard_pop();
                    }
                }
                inserted = false;
                break;
            }
            else
            {
                assert(partial_function.total_function == generalization.total_function);
                partial_function.partition |= generalization_mask;
            }
        }
    }

    if(inserted)
    {
        contained_meta_examples.push_back(meta_example);
    }
    else
    {
        inconsistent_meta_examples.push_back(meta_example);
    }
}

void SubdomainSwitchPosetsFactors::print()
{
    for(int i = 0;i<factors.size();i++)
    {
        cout << "factor i = " << i << endl;
        for(int j = 0;j<factors[i].subdomain_posets.size();j++)
        {
            cout << factors[i].masks_to_string(j) << " num_meta_edges = " << factors[i].subdomain_posets[j].get_num_meta_edges() <<" ";
            factors[i].subdomain_posets[j].soft_delete_redundant_edges();
            cout << "necessary_meta_edges = " << factors[i].subdomain_posets[j].get_num_meta_edges() << endl;
        }
        cout << endl;
    }
    cout << endl;

    cout << "contained_meta_examples.size() " << contained_meta_examples.size() << endl;
    cout << "inconsistent_meta_examples.size() " << inconsistent_meta_examples.size() << endl;
}

ReasoningSchema::ReasoningSchema(int num_inputs, int generalization_mask, int training_mask, vector<MetaExample> meta_examples) {
    schema_type = leaf_schema;
    compact_poset = CompactPoset(num_inputs, generalization_mask, training_mask);
    for(int i = 0;i<meta_examples.size();i++)
    {
        assert(compact_poset.insert(meta_examples[i]));
    }
}

ReasoningSchema::ReasoningSchema(int _num_inputs, vector<MetaExample> train_meta_examples, vector<pair<int, int> > masks, vector<pair<int, int> > training_masks)
{
    time_t local_time = time(nullptr);
    num_inputs = _num_inputs;
    function_size = (1<<num_inputs);

    assert(masks.size()==training_masks.size());
    if(train_meta_examples.size() == 0)
    {
        schema_type = leaf_schema;
        compact_poset = CompactPoset(num_inputs);

        cout << "UNIT LEAF SCHEMA" << endl;

        return;
    }


    vector<pair<CompactPoset, CompactPoset> > subdomain_poset;// = vector<pair<CompactPoset, CompactPoset>>(masks.size(), make_pair(CompactPoset(num_inputs), CompactPoset(num_inputs)));

    for(int i = 0;i<masks.size();i++)
    {
        subdomain_poset.push_back(
                make_pair(
                        CompactPoset(num_inputs, masks[i].first, training_masks[i].first),
                        CompactPoset(num_inputs, masks[i].second, training_masks[i].second))
                );
    }

    vector<vector<MetaExample> > contained_meta_examples = vector<vector<MetaExample> >(masks.size(), vector<MetaExample>());
    vector<vector<MetaExample> > inconsistent_meta_examples = vector<vector<MetaExample> >(masks.size(), vector<MetaExample>());

    vector<SubdomainSwitchPosetsFactors> subdomain_switch_posets_factors;
    for(int i = 0;i<masks.size();i++)
    {
        subdomain_switch_posets_factors.push_back(SubdomainSwitchPosetsFactors(num_inputs, masks[i], training_masks[i]));
    }

    cout << endl;
    cout << "INSERT " << train_meta_examples.size() << " meta examples in ReasoningSchema" <<endl;

    time_t prev_time = time(nullptr);

    for(int i = (int) 0; i < train_meta_examples.size(); i++)
    {
        PartialFunction local_partial_function = train_meta_examples[i].partial_function;
        PartialFunction generalization = train_meta_examples[i].generalization;

//        cout << "insert " << train_meta_examples[i].to_string() << endl;

        if(local_partial_function.partition_size() < function_size) {

            for(int k = 0;k <subdomain_switch_posets_factors.size();k++)
            {
                train_meta_examples[i].idx = i;
                subdomain_switch_posets_factors[k].insert(train_meta_examples[i]);
            }

            assert((generalization.partition & local_partial_function.partition) == local_partial_function.partition);
            int main_subdomain = generalization.partition - local_partial_function.partition;

            vector<int> mask_ids;

            for(int k = 0;k < masks.size();k++)
            {
                if((masks[k].first & main_subdomain) != 0 || (masks[k].second & main_subdomain) != 0) {
                    mask_ids.push_back(k);
                }
            }

            for (int k = 0; k < mask_ids.size(); k++) {
                int first_mask = masks[mask_ids[k]].first;
                int second_mask = masks[mask_ids[k]].second;
                int first_local_partition = (training_masks[mask_ids[k]].first - (main_subdomain & first_mask)) & local_partial_function.partition;
                int second_local_partition = (training_masks[mask_ids[k]].second - (main_subdomain & second_mask)) & local_partial_function.partition;
                first_mask |= first_local_partition;
                second_mask |= second_local_partition;

                assert(((masks[mask_ids[k]].first | training_masks[mask_ids[k]].first) & first_mask) == first_mask);
                assert(((masks[mask_ids[k]].second | training_masks[mask_ids[k]].second) & second_mask) == second_mask);

                MetaExample first_partial_meta_example = MetaExample(
                        num_inputs,
                        train_meta_examples[i].generalization.total_function,
                        first_local_partition, first_mask, i);
                MetaExample second_partial_meta_example = MetaExample(
                        num_inputs,
                        train_meta_examples[i].generalization.total_function,
                        second_local_partition, second_mask, i);


//                cout << first_partial_meta_example.to_string() << " "<< second_partial_meta_example.to_string() << endl;

                bool is_contained = false;
                if(first_mask != first_local_partition) {
                    if (!subdomain_poset[mask_ids[k]].first.insert(first_partial_meta_example)) {
                        subdomain_poset[mask_ids[k]].first.hard_pop();
                        inconsistent_meta_examples[mask_ids[k]].push_back(train_meta_examples[i]);
                    }
                    else
                    {
                        if(second_mask!=second_local_partition)
                        {
                            if (!subdomain_poset[mask_ids[k]].second.insert(second_partial_meta_example)) {
                                subdomain_poset[mask_ids[k]].second.hard_pop();
                                subdomain_poset[mask_ids[k]].first.hard_pop();
                                inconsistent_meta_examples[mask_ids[k]].push_back(train_meta_examples[i]);
                            }
                            else
                            {
//                                cout << "A" << endl;
                                is_contained = true;
                            }
                        }
                        else
                        {
//                            cout << "B" << endl;
                            is_contained = true;
                        }
                    }
                }
                else
                {
                    if(second_mask!=second_local_partition)
                    {
                        if (!subdomain_poset[mask_ids[k]].second.insert(second_partial_meta_example)) {
                            subdomain_poset[mask_ids[k]].second.hard_pop();
                            inconsistent_meta_examples[mask_ids[k]].push_back(train_meta_examples[i]);
                        }
                        else
                        {
//                            cout << "C" << endl;
                            is_contained = true;
                        }
                    }
                    else
                    {
//                        cout << "D" << endl;
                        is_contained = true;
                    }
                }

                if(is_contained)
                {
                    contained_meta_examples[mask_ids[k]].push_back(train_meta_examples[i]);

////                    cout << "meta_example" << endl;
////                    train_meta_examples[i].print();
////
////                    cout << "first_partial_meta_example" << endl;
////                    first_partial_meta_example.print();
////
////                    cout << "second_partial_meta_example" << endl;
////                    second_partial_meta_example.print();
////
////                    cout << "factoring (generalization): " << bitvector_to_str(masks[mask_ids[k]].first, function_size) << " " << bitvector_to_str(masks[mask_ids[k]].second, function_size) << endl;
////                    cout << "factoring (training      ): " << bitvector_to_str(training_masks[mask_ids[k]].first, function_size) << " " << bitvector_to_str(training_masks[mask_ids[k]].second, function_size) << endl;
//
//                    subdomain_poset[mask_ids[k]].first.calc_dominator_unions();
//                    vector<PartialFunction> left_result = subdomain_poset[mask_ids[k]].first.query(train_meta_examples[i].partial_function);
////                    cout << "left_result:" << endl;
////                    for(int k = 0; k < left_result.size(); k++)
////                    {
////                        cout << left_result[k].to_string() << endl;
////                    }
//
//                    subdomain_poset[mask_ids[k]].second.calc_dominator_unions();
//                    vector<PartialFunction> right_result = subdomain_poset[mask_ids[k]].second.query(train_meta_examples[i].partial_function);
////                    cout << "right_result:" << endl;
////                    for(int k = 0; k < right_result.size(); k++)
////                    {
////                        cout << right_result[k].to_string() << endl;
////                    }
//
//                    vector<PartialFunction> ret;
//                    for(int k = 0;k<left_result.size();k++)
//                    {
//                        for(int l = 0;l<right_result.size();l++)
//                        {
//                            PartialFunction composed_result = left_result[k].get_composition(right_result[l]);
//                            ret.push_back(composed_result);
//                        }
//                    }
//
//                    assert(ret.size() == 1);
//                    assert((ret[0].partition & train_meta_examples[i].generalization.partition) == train_meta_examples[i].generalization.partition);
//                    assert(ret[0].total_function == train_meta_examples[i].generalization.total_function);

                }
//                      cout << partial_meta_example.to_string() << " mask_ids[k] = " << bitvector_to_str(mask_ids[k], function_size) << " num_meta_edges = " << subdomain_poset[mask_ids[k]].get_num_meta_edges() << " num_unhandled = " << inconsistent_meta_examples[mask_ids[k]].size() << endl;
            }
        }
        if(time(nullptr) - prev_time >= 3) {
            cout
                << "i = " << i
                << " num_decision_tree_nodes = " << get__global_num_decision_tree_nodes()
                << " num_empty_slots = " << get__empty_slots_count()
                << " time: " << (double)time(nullptr) - local_time << endl;
            prev_time = time(nullptr);
        }
    }

    for(int i = 0;i<subdomain_switch_posets_factors.size();i++)
    {
        subdomain_switch_posets_factors[i].print();
    }

    vector<pair<int, int> > best_factorizations;
    pair<int, int> best_factorization = make_pair(3*train_meta_examples.size(), -1);

    cout << endl;
    cout << "ANALYZE FACTORING" <<endl;

    vector<CompactPoset> remainders = vector<CompactPoset>(subdomain_poset.size(), CompactPoset(num_inputs));

    for(int j = 0; j<subdomain_poset.size();j++)
    {
        cout << endl;
        cout << "consistent factoring (generalization): " << bitvector_to_str(masks[j].first, function_size) << " " << bitvector_to_str(masks[j].second, function_size) << endl;
        cout << "consistent factoring (training      ): " << bitvector_to_str(training_masks[j].first, function_size) << " " << bitvector_to_str(training_masks[j].second, function_size) << endl;
        subdomain_poset[j].first.soft_delete_redundant_edges();
        cout << "left necessary = " << subdomain_poset[j].first.get_num_meta_edges() << endl;
        cout << "left no duplicates = " << subdomain_poset[j].first.get_all_meta_examples_without_duplicates().size() << endl;

        subdomain_poset[j].second.soft_delete_redundant_edges();
        cout << "right necessary = " <<subdomain_poset[j].second.get_num_meta_edges() << endl;
        cout << "right no duplicates = " << subdomain_poset[j].second.get_all_meta_examples_without_duplicates().size() << endl;

        cout << "contained meta examples = " << contained_meta_examples[j].size() << endl;
//
        cout << "num_meta_examples_to_insert_in_remainder = " << inconsistent_meta_examples[j].size() << endl;

        for(int k = 0; k<inconsistent_meta_examples[j].size();k++)
        {
            assert(remainders[j].insert(inconsistent_meta_examples[j][k]));
        }

//        for(int k = 0;k<contained_meta_examples[j].size();k++)
//        {
//            subdomain_poset[j].first.calc_dominator_unions();
//            vector<PartialFunction> left_result = subdomain_poset[j].first.query(contained_meta_examples[j][k].partial_function);
//
//
//            subdomain_poset[j].second.calc_dominator_unions();
//            vector<PartialFunction> right_result = subdomain_poset[j].second.query(contained_meta_examples[j][k].partial_function);
//
//
//            vector<PartialFunction> ret;
//            for(int l = 0;l<left_result.size();l++)
//            {
//                for(int m = 0;m<right_result.size();m++)
//                {
//                    PartialFunction composed_result = left_result[l].get_composition(right_result[m]);
//                    ret.push_back(composed_result);
//                }
//            }
//
//            if(ret.size()>=2)
//            {
//                cout << contained_meta_examples[j][k].to_string() << endl;
//                    cout << "left_result:" << endl;
//                    for(int k = 0; k < left_result.size(); k++)
//                    {
//                        cout << left_result[k].to_string() << endl;
//                    }
//                    cout << "right_result:" << endl;
//                    for(int k = 0; k < right_result.size(); k++)
//                    {
//                        cout << right_result[k].to_string() << endl;
//                    }
//                cout<< "first" << endl<<endl <<subdomain_poset[j].first.meta_examples_to_string()<<endl;
//                cout<<"second" << endl<<endl << subdomain_poset[j].second.meta_examples_to_string()<<endl;
//            }
//
//            assert(ret.size() == 1);
//            assert((ret[0].partition & contained_meta_examples[j][k].generalization.partition) == contained_meta_examples[j][k].generalization.partition);
//            assert(ret[0].total_function == contained_meta_examples[j][k].generalization.total_function);
//
//        }

//        for(int k = 0; k<contained_meta_examples[j].size();k++)
//        {
//            vector<PartialFunction> result = remainders[j].query(contained_meta_examples[j][k].partial_function);
//            PartialFunction intermediate_result = result[0];
//            for(int l = 1; l < result.size(); l++)
//            {
//                intermediate_result.apply_intersection(result[l]);
//            }
//            if(contained_meta_examples[j][k].partial_function.partition != intermediate_result.partition)
//            {
//                MetaExample local_meta_example = MetaExample(
//                        num_inputs,
//                        intermediate_result.total_function,
//                        contained_meta_examples[j][k].partial_function.partition,
//                        intermediate_result.partition);
//                assert(remainders[j].insert(local_meta_example));
//            }
//        }

        remainders[j].soft_delete_redundant_edges();
        cout << "remainder necessary = " << remainders[j].get_num_meta_edges() << " num_decision_tree_nodes "<<  get__global_num_decision_tree_nodes() << " empty slots "<< get__empty_slots_count()  << " time: " << (double)time(nullptr) - local_time << endl;

        int remainder_without_duplicates_count = (int) remainders[j].get_all_meta_examples_without_duplicates().size();

        cout << "remainder withouth duplicates = " << remainder_without_duplicates_count << endl;

        int local_num_meta_examples_of_decomposition = remainders[j].get_num_meta_edges() + subdomain_poset[j].first.get_num_meta_edges() + subdomain_poset[j].second.get_num_meta_edges();

        best_factorizations.push_back(make_pair(local_num_meta_examples_of_decomposition, j));

        int prev_best_j = best_factorization.second;

        best_factorization = min(best_factorization, make_pair(local_num_meta_examples_of_decomposition, j));

        if(best_factorization.second != j)
        {
            remainders[j].clear();
        }
        else if(prev_best_j != -1)
        {
            remainders[prev_best_j].clear();
        }

    }

    sort(best_factorizations.begin(), best_factorizations.end());

    cout << endl;

    int best_subdomain = best_factorization.second;

    assert(best_subdomain != -1);

    min_num_necessary_meta_examples = best_factorization.first;

    cout << "BEST FACTORIZATION = " << bitvector_to_str(masks[best_subdomain].first, function_size) <<" "<< bitvector_to_str(masks[best_subdomain].second, function_size) <<" num_meta_examples " << best_factorization.first << endl;

    cout << "SORT FACTORINGS::" <<endl;
    for(int j = 0;j<best_factorizations.size();j++)
    {
        int k = best_factorizations[j].second;
        cout << "size = " <<  best_factorizations[j].first << endl;
        cout << "consistent factoring (generalization): " << bitvector_to_str(masks[k].first, function_size) << " " << bitvector_to_str(masks[k].second, function_size) << endl;
        cout << "consistent factoring (training      ): " << bitvector_to_str(training_masks[k].first, function_size) << " " << bitvector_to_str(training_masks[k].second, function_size) << endl;
        cout << endl;
    }
    cout << endl;

    vector<pair<int, int> > root_schema__masks = masks;

    vector<vector<pair<int, int> > > factor_schema__submasks = vector<vector<pair<int, int> > >(2, vector<pair<int, int> >());


    vector<int> factor_schema__masks;
    factor_schema__masks.push_back(masks[best_subdomain].first);
    factor_schema__masks.push_back(masks[best_subdomain].second);

//    for(int i = 0;i<factor_schema__masks.size();i++)
//    {
//        int num_bits_in_mask = __builtin_popcount(factor_schema__masks[i]);
//        int num_submasks = (1<<num_bits_in_mask);
//
//        cout << endl;
//        cout << "MASK FOR FACTOR i = " << i << endl;
//
//        for(int j = 1;j<num_submasks/2;j++)
//        {
//            int new_mask = 0;
//            for(int k = 0, l = 0;k<function_size;k++)
//            {
//                if(get_bit(factor_schema__masks[i], k))
//                {
//                    new_mask |= (get_bit(j, l) << k);
//                    l++;
//                }
//            }
//            int new_mask_complement = factor_schema__masks[i]-new_mask;
//            factor_schema__submasks[i].push_back(make_pair(new_mask, new_mask_complement));
//            cout << bitvector_to_str(new_mask, function_size) <<" "<< bitvector_to_str(new_mask_complement, function_size) << endl;
//        }
//    }

//    cout << "BEST LEFT: " << endl;
//    cout << subdomain_poset[best_subdomain].first.meta_examples_to_string() << endl;
//    cout << "BEST RIGHT: " << endl;
//    cout << subdomain_poset[best_subdomain].second.meta_examples_to_string() << endl;
//    cout << "BEST REMAINDER: " << endl;
//    cout << remainders[best_subdomain].meta_examples_to_string() << endl;

//    vector<MetaExample> remainder_meta_examples = remainders[best_subdomain].get_all_meta_examples_without_duplicates();

    schema_type = factorization_schema;

    int total_domain = (1<<(1<<num_inputs))-1;

    root_schema = new ReasoningSchema(num_inputs, total_domain, total_domain,
                                      remainders[best_subdomain].get_existant_meta_examples());

    factor_schemas = make_pair(
            new ReasoningSchema(
                    num_inputs,
                    masks[best_subdomain].first,
                    training_masks[best_subdomain].first,
                    subdomain_poset[best_subdomain].first.get_existant_meta_examples()),
            new ReasoningSchema(
                    num_inputs,
                    masks[best_subdomain].second,
                    training_masks[best_subdomain].second,
                    subdomain_poset[best_subdomain].second.get_existant_meta_examples())
            );



    for(int j = 0; j<subdomain_poset.size();j++)
    {
        subdomain_poset[j].first.clear();
        subdomain_poset[j].second.clear();
    }

    for(int j = 0;j<remainders.size();j++)
    {
        remainders[j].clear();
    }

    subdomain_poset.clear();

    remainders.clear();

}

bool ReasoningSchema::test(vector<MetaExample> test_meta_examples) {

    for(int i = 0;i<test_meta_examples.size();i++)
    {
//        cout << "meta_example:" << endl;
//        test_meta_examples[i].print();
        vector<PartialFunction> result = query(test_meta_examples[i].partial_function);
//        cout << "result:" << endl;
//        for(int k = 0; k < result.size(); k++)
//        {
//            cout << result[k].to_string() << endl;
//        }
        if(result.size() == 1)
        {
            if(!result[0].is_generalization_of(test_meta_examples[i].generalization))
            {
                cout << "WRONG" <<endl;
                return false;
            }
            else
            {
//                cout << "OK" <<endl;
            }
        }
        else
        {
//            cout << "first:" << endl;
//            factor_schemas.first->compact_poset.print();
//            cout << "second:" << endl;
//            factor_schemas.second->compact_poset.print();
            cout << "WRONG" <<endl;
            return false;
        }
    }
    cout << "TEST COMPLETE" << endl;
    return true;
}

vector<PartialFunction> ReasoningSchema::query(PartialFunction partial_function)
{
    if(schema_type == leaf_schema)
    {
        return compact_poset.query(partial_function);
    }
    else
    {
        vector<PartialFunction> result = root_schema->query(partial_function);
//        cout << "root_schema result:" << endl;
//        for(int k = 0; k < result.size(); k++)
//        {
//            cout << result[k].to_string() << endl;
//        }
        if(result.size() == 1 && __builtin_popcount(result[0].partition) == function_size) {
            return result;
        }
        else
        {
            PartialFunction intermediate_result = result[0];
            for(int k = 1; k < result.size(); k++)
            {
                intermediate_result.apply_intersection(result[k]);
            }
//            cout << "intermediate_result = " << intermediate_result.to_string() << endl;
            vector<PartialFunction> left_result = factor_schemas.first->query(intermediate_result);
//            cout << "left_result:" << endl;
//            for(int k = 0; k < left_result.size(); k++)
//            {
//                cout << left_result[k].to_string() << endl;
//            }
            vector<PartialFunction> right_result = factor_schemas.second->query(intermediate_result);
//            cout << "right_result:" << endl;
//            for(int k = 0; k < right_result.size(); k++)
//            {
//                cout << right_result[k].to_string() << endl;
//            }

            vector<PartialFunction> ret;
            for(int k = 0;k<left_result.size();k++)
            {
                for(int l = 0;l<right_result.size();l++)
                {
                    PartialFunction composed_result = left_result[k].get_composition(right_result[l]);
                    ret.push_back(composed_result);
                }
            }

            return ret;
        }
    }
}

int ReasoningSchema::get_num_necessary_meta_examples() {
    return min_num_necessary_meta_examples;
}


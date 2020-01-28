#include <iostream>
#include "CompactPoset.h"
#include "LanguagesOverBooleanFunctions.h"
#include <queue>
#include "RemainderAndTwoFactorsSchema.h"
#include "MinimalFactoringSchema.h"
#include "UnionOfPartialFunctions.h"
#include "enumerate_meta_training_sets.h"
#include "BitvectorTasks.h"

vector<MetaExample> get_meta_examples_of_language_over_boolean_functions(int language_id, int num_inputs, int min_partition_size, int max_partition_size, int macro_partition)
{
    vector<MetaExample> meta_examples;

    LanguagesOverBooleanFunctions language = LanguagesOverBooleanFunctions(num_inputs, language_id);
    language.enumerate();

    int function_size = (1 << num_inputs);
    int num_functions = (1 << function_size);
    int num_partitions = num_functions-1;

    if(macro_partition != -1)
    {
        num_partitions = (1<<macro_partition);
    }

    auto partitions_by_size = vector<vector<Bitvector> >(function_size, vector<Bitvector>());
    auto meta_examples_per_total_function = vector<vector<MetaExample> >(num_functions, vector<MetaExample>());

//            auto meta_examples_per_total_function = vector<DecisionTree>(num_functions, PartialFunction(function_size, 0, 0));


    for (Bitvector partition = 0; partition < num_partitions; partition++) {
        partitions_by_size[partition.count()].push_back(partition);
    }
    if(max_partition_size == -1)
    {
        max_partition_size = (int) partitions_by_size.size()-1;
    }
    if(min_partition_size == -1)
    {
        min_partition_size = 0;
    }

    cout << "enumerate meta examples for language_" << language_id << endl;
//            cout << "partial -> total" << endl;

    int local_total_num_meta_examples = 0;
    int local_total_without_trivial = 0;

    for(int partition_size = min_partition_size;partition_size<=max_partition_size;partition_size++) {
//                cout << "partition_size = " << partition_size << " time: " << (double)time(nullptr) - local_time << endl;
        for (int partition_idx = 0; partition_idx < partitions_by_size[partition_size].size(); partition_idx++) {
            Bitvector partition = partitions_by_size[partition_size][partition_idx];
            int num_partition_assignments = (1 << partition_size);
            for (int partial_assignment = 0;
                 partial_assignment < num_partition_assignments; partial_assignment++) {
                Bitvector total_function = 0;
                int j = 0;
                for (int i = 0; i < function_size; i++) {
                    if (get_bit(partition, i)) {
                        total_function.set(i, get_bit(partial_assignment, j));
                        j++;
                    }
                }
                MetaExample local_meta_example =
                        language.get_meta_example(PartialFunction((1<<num_inputs), total_function, partition));

                bool add = !local_meta_example.fully_constrained();
                for(int i = 0; i<meta_examples_per_total_function[local_meta_example.generalization.total_function.to_ullong()].size(); i++)
                {
                    MetaExample smaller_meta_example = meta_examples_per_total_function[local_meta_example.generalization.total_function.to_ullong()][i];

                    if(local_meta_example.partial_function.is_contained_in(
                            smaller_meta_example.partial_function))
                    {
                        add = false;
                        break;
                    }
                }

//                    cout << "is_generalization_of_counter " << language.is_generalization_counter <<endl;
//                local_meta_example.print();
                local_total_num_meta_examples++;
                if(add) {
                    assert(local_meta_example.generalization.partition != local_meta_example.partial_function.partition);
                    local_total_without_trivial++;
//                            local_meta_example.print();
                    local_meta_example.idx = (int) meta_examples.size();
                    meta_examples.push_back(local_meta_example);
                    meta_examples_per_total_function[local_meta_example.generalization.total_function.to_ullong()].push_back(local_meta_example);
//                            meta_examples_per_total_function[local_meta_example.generalization.generalization].apply_operation(
//                                    my_union, new DecisionTree(local_meta_example.partial_function));
                }
            }
        }
//                cout << "partition_size = " << partition_size << " num_of_meta_examples = " << meta_examples.size() << " time: " << (double)time(nullptr) - local_time << endl;
    }

    return meta_examples;
}

void monolith_test(int num_inputs, vector<MetaExample> meta_examples)
{

    int prev_num_meta_examples = meta_examples.size();
    cout << "INSERT " << meta_examples.size() << " meta edges in CompactPoset" << endl;
    CompactPoset poset = CompactPoset((1<<num_inputs), meta_examples);
    assert(meta_examples.size() == prev_num_meta_examples);
    int prev_num_meta_edges = poset.get_num_meta_edges();
    poset.soft_delete_redundant_edges();
//    cout << "classification of meta examples for language_" << language_id << " :: " << endl;
//    cout << "partial -> total  classification" << endl;
//    cout << poset.meta_examples_to_string() << endl;
//
//    tests model stored in poset.
    poset.calc_dominator_unions();
    poset.calc_downstream_unions();

    contains_counter = 0;

    poset.num_visited_nodes_during_query = 0;

    for(int j = 0;j<meta_examples.size();j++)
    {
        vector<PartialFunction> result = poset.query(meta_examples[j].partial_function);
        cout << "meta_example:" << endl;
        meta_examples[j].print();
        cout << "result:" << endl;
        for(int k = 0; k < result.size(); k++)
        {
            cout << result[k].to_string() << endl;
        }
        assert(result.size() == 1);
        assert(result[0].total_function == meta_examples[j].generalization.total_function);
    }

    cout << "poset.num_visited_nodes_during_query = " << poset.num_visited_nodes_during_query <<endl;

    cout << "contains_counter = " << contains_counter << endl;


    poset.print();

    int diff_in_meta_edges = (prev_num_meta_edges - poset.get_num_meta_edges());

    cout << "DONE "
         << " num_nodes = " << poset.get_num_nodes()
         << " num_remaining_meta_edges = " << poset.get_num_meta_edges()
         << " num_inserted_meta_edges = " << poset.get_num_inserts()
         << " num_decision_tree_nodes = " << get__global_num_decision_tree_nodes()
         << " num_empty_slots = " << get__empty_slots_count()
         << " time: " << (double)time(nullptr) - local_time << endl;
    cout << "redundant edges = " << diff_in_meta_edges << endl;


    poset.clear();
}

void remainder_and_two_factors_schema(int num_inputs, int function_size, vector<MetaExample> meta_examples)
{

    vector<pair<Bitvector, Bitvector> > masks;
    vector<pair<Bitvector, Bitvector> > partition_masks;

    for (int j = 0; j < num_inputs; j++) {
        int mask = 0;
        for (int k = 0; k < function_size; k++) {
            if (get_bit(k, j)) {
                mask |= (1 << k);
            }
        }
        masks.push_back(make_pair(mask, (1 << function_size) - 1 - mask));
        partition_masks.push_back(make_pair(mask, (1 << function_size) - 1 - mask));

    }


    int init_masks_size = masks.size();
    for (int j = 0; j < init_masks_size; j++) {
//                masks.push_back(make_pair(masks[j].first, masks[j].second));
//                partition_masks.push_back(make_pair((1<<function_size)-1, (1<<function_size)-1));

        masks.push_back(make_pair(masks[j].first, masks[j].second));
        partition_masks.push_back(make_pair((1 << function_size) - 1 - 1, (1 << (function_size)) - 1));
        for (int k = j + 1; k < init_masks_size; k++) {

            masks.push_back(make_pair(masks[j].first, masks[j].second));
            partition_masks.push_back(
                    make_pair(masks[j].first | masks[k].first, masks[j].second | masks[k].second));
//
//                    masks.push_back(make_pair(masks[j].first|masks[k].first, masks[j].second|masks[k].second));
//                    partition_masks.push_back(make_pair(masks[j].first|masks[k].first, masks[j].second|masks[k].second));
//
//                    masks.push_back(make_pair(masks[j].first|masks[k].first, masks[j].second|masks[k].second));
//                    partition_masks.push_back(make_pair((1<<function_size) - 1, (1<<function_size) - 1));
//
            masks.push_back(make_pair(masks[k].first, masks[k].second));
            partition_masks.push_back(
                    make_pair(masks[j].first | masks[k].first, masks[j].second | masks[k].second));
//
//                    masks.push_back(make_pair(masks[j].first|masks[k].first, masks[j].second|masks[k].second));
//                    partition_masks.push_back(make_pair(masks[j].first|masks[k].first, masks[j].second|masks[k].second));
//
//                    masks.push_back(make_pair(masks[j].first|masks[k].first, masks[j].second|masks[k].second));
//                    partition_masks.push_back(make_pair((1<<function_size) - 1, (1<<function_size) - 1));
        }
    }

//            masks.push_back(make_pair((1<<function_size) - 1 - 1, (1<<(function_size-1)) - 1));
//            partition_masks.push_back(make_pair((1<<function_size) - 1, (1<<function_size) - 1));
//            masks.push_back(make_pair((1<<function_size) - 1, (1<<(function_size)) -1));
//            partition_masks.push_back(make_pair((1<<function_size) - 1, (1<<function_size) - 1));
//
//            masks.erase(masks.begin(), masks.begin()+init_masks_size);
//            partition_masks.erase(partition_masks.begin(), partition_masks.begin()+init_masks_size);
//
//            masks.push_back(make_pair((1<<function_size)-1-1, (1<<(function_size-1))-1));

    cout << "masks & partition_masks:" << endl;
    for (int j = 0; j < masks.size(); j++) {
        cout << bitvector_to_str(masks[j].first, function_size) << " "
             << bitvector_to_str(masks[j].second, function_size) << endl;
        cout << bitvector_to_str(partition_masks[j].first, function_size) << " "
             << bitvector_to_str(partition_masks[j].second, function_size) << endl;
        cout << endl;
    }

//            for(int j = 0;j<num_functions;j++)
//            {
//                masks.push_back(j);
//                cout << bitvector_to_str(j, function_size) << endl;
//            }

    RemainderAndTwoFactorsSchema reasoning_schema = RemainderAndTwoFactorsSchema((1<<num_inputs), meta_examples, masks, partition_masks);

    reasoning_schema.test(meta_examples);
}

void recursive_minimal_factoring_schema(int num_inputs, int language_id, vector<MetaExample> meta_examples)
{
    vector<MetaExample> local_meta_examples = meta_examples;
    int prev_meta_examples_size = -1;
    int now_meta_examples_size = (int) local_meta_examples.size();
    int rec_id = 0;
    do{
        prev_meta_examples_size = now_meta_examples_size;

        string language_name =
                "HERE_language(n=" + std::to_string(num_inputs) + ",id=" + std::to_string(language_id) + ")[rec="+std::to_string(rec_id)+"]";

        MinimalFactoringSchema my_schema =
                MinimalFactoringSchema(local_meta_examples, language_name, true || (rec_id != 0));

        for(int i = 0;i<local_meta_examples.size();i++)
        {
            PartialFunction generalization = my_schema.query(local_meta_examples[i].partial_function);
            cout << "query  " << local_meta_examples[i].to_string() << endl;
            cout << "result " << generalization.to_string() << endl;
            cout << endl;
            assert(generalization.is_contained_in(local_meta_examples[i].generalization));
        }
        cout << "TESTING DONE. ALL CORRECT" << endl;

        local_meta_examples = my_schema.get_necessary_meta_examples(false);

        now_meta_examples_size = (int) local_meta_examples.size();
        rec_id++;

    }while(now_meta_examples_size != prev_meta_examples_size);
}

void modeling_of_language_over_boolean_functions()
{
    for(int language_id = 0; language_id < 2;language_id++)
    {

        int num_inputs = 3;
        int min_partition_size = -1;
        int max_partition_size = -1;
        int macro_partition = -1;

        int function_size = (1 << num_inputs);

        vector<MetaExample> meta_examples = get_meta_examples_of_language_over_boolean_functions(
                language_id, num_inputs, min_partition_size, max_partition_size, macro_partition);

        cout << "enumerated " << meta_examples.size() <<" meta examples for language_" << language_id << endl;

        bool insert_in_monolith = false;
        if(insert_in_monolith)
        {
            monolith_test(num_inputs, meta_examples);
        }

        bool run_remainder_and_two_factors_schema = false;

        if(run_remainder_and_two_factors_schema) {
            remainder_and_two_factors_schema(num_inputs, function_size, meta_examples);
        }

        bool run_recursive_minimal_factoring_schema = false;

        if(run_recursive_minimal_factoring_schema) {
            recursive_minimal_factoring_schema(num_inputs, language_id, meta_examples);
        }
    }
}

void modeling_of_bitvector_functions()
{
    BitvectorTasks bitvector_tasks = BitvectorTasks();
}

int main() {

    bool do__enumerate_sets_of_meta_examples = false;
    if(do__enumerate_sets_of_meta_examples)
    {
        enumerate_meta_training_sets();
    }

    bool do__modeling_of_language_over_boolean_functions = false;
    if(do__modeling_of_language_over_boolean_functions)
    {
        modeling_of_language_over_boolean_functions();
    }

    bool do__modeling_of_bitvector_functions = true;
    if(do__modeling_of_bitvector_functions)
    {
        modeling_of_bitvector_functions();
    }

    cout << "TIME: " << (double) time(nullptr) - local_time << endl;

    return 0;
}


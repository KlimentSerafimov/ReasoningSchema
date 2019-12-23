#include <iostream>
#include "CompactPoset.h"
#include <fstream>
#include "LanguagesOverBooleanFunctions.h"
#include <queue>
#include "ReasoningSchema.h"
#include "MinimalFactoringSchema.h"



int min_num_examples;
int max_num_examples;

class AllMetaExamples
{
public:
    int num_inputs;
    unsigned int function_size;
    unsigned int num_functions;

    vector<vector<vector<MetaExample> > > meta_examples;
    vector<vector<vector<vector<MetaExample> > > > skip_meta_examples;
    vector<vector<int> > ordering_of_partition_per_size;

    AllMetaExamples()
    {

    }

    explicit AllMetaExamples(int _n, bool print)
    {
        num_inputs = _n;

        function_size = (unsigned int) (1 << num_inputs);
        num_functions = (unsigned int) (1 << function_size);
        int num_partitions = (1 << function_size);

        for (int partition = 0; partition < num_partitions; partition++)
        {
            int size_of_partial_function = __builtin_popcount(partition);
            auto num_of_partial_functions = (unsigned int) (1 << size_of_partial_function);
            meta_examples.emplace_back(vector<vector<MetaExample> >(num_of_partial_functions));
            skip_meta_examples.emplace_back(vector<vector<vector<MetaExample> > >(num_of_partial_functions));

            for(int output_bits = 0; output_bits < num_functions; output_bits ++)
            {
                MetaExample local_meta_example = MetaExample(num_inputs, output_bits, partition);
                int compact_partial_outputs = local_meta_example.get_compact_partial_outputs();

                meta_examples[partition][compact_partial_outputs].push_back(local_meta_example);
                skip_meta_examples[partition][compact_partial_outputs].push_back(vector<MetaExample>());

//                int size_of_generalization = function_size - size_of_partial_function;
//                int num_partition_expansions = (1<<size_of_generalization);
//
//                int local_generalization_id = (int) skip_meta_examples[partition][compact_partial_outputs].size()-1;
//                assert(skip_meta_examples[partition][compact_partial_outputs][local_generalization_id].empty());
//                for(int partition_expansion = 1; partition_expansion < num_partition_expansions; partition_expansion++)
//                {
//                    int over_partition = partition;
//                    for(int i = 0, j=0;i<function_size;i++)
//                    {
//                        if(!get_bit(partition, i))
//                        {
//                            over_partition += get_bit(partition_expansion, j)*(1<<i);
//                            j++;
//                        }
//                    }
//                    MetaExample exstended_meta_example = MetaExample(num_inputs, output_bits, over_partition);
//
//                    skip_meta_examples[partition][compact_partial_outputs][local_generalization_id].push_back(
//                            exstended_meta_example
//                    );
//                }
//                assert(skip_meta_examples[partition][compact_partial_outputs][local_generalization_id].size() == num_partition_expansions-1);
            }
        }

        ordering_of_partition_per_size = vector<vector<int> >(function_size + 1);

        for (int partition = 0; partition < meta_examples.size(); partition++) {
            ordering_of_partition_per_size[__builtin_popcount(partition)].push_back(partition);
        }

        if(print) {

            for (int partition = 0; partition < meta_examples.size(); partition++) {
                int size_of_partial_function = __builtin_popcount(partition);
                for (int j = 0; j < meta_examples[partition].size(); j++) {
                    for (int k = 0; k < meta_examples[partition][j].size(); k++) {
                        int generalization_output = meta_examples[partition][j][k].get_compact_hidden_outputs();
                        cout
                                << bitvector_to_str(partition, (1 << num_inputs)) << " "
                                << bitvector_to_str(j, size_of_partial_function) << " "
                                //                        << bitvector_to_str(generalization_output, (1<<num_inputs)-size_of_partial_function) << endl;
                                << meta_examples[partition][j][k].to_string() << endl;
                        cout << "SKIP:" << endl;
                        for (int l = 0; l < skip_meta_examples[partition][j][k].size(); l++) {

                            cout << skip_meta_examples[partition][j][k][l].linear_string(1) << endl;
                        }
                    }
                    cout << endl;
                }
            }
        }

    }

};

class MetaExampleCoordinate
{
public:
    int partition_id;
    int partial_assignment_id;
    int generalization_id;

    MetaExampleCoordinate(int _partition_id, int _partial_assignment_id, int _generalization_id)
    {
        partition_id = _partition_id;
        partial_assignment_id = _partial_assignment_id;
        generalization_id = _generalization_id;
    }

    string to_string()
    {
        return std::to_string(partition_id) + " " + std::to_string(partial_assignment_id) + " " + std::to_string(generalization_id);
    }
};

AllMetaExamples all_meta_examples = AllMetaExamples();


ofstream current_file;


int num_dones = 0;
int num_early_invalids = 0;
int sum_saved = 0;
vector<int> leafs_saved_per_invalid;
int num_calls = 0;

time_t local_time = time(nullptr);

int DEPTH_TREEE_PRINT;
int FREQUENCY_DONE_PRINT;

void enumerate_meta_training_sets(
        int partition_size,
        int local_partition_index,
        int partial_assignment_id,
        CompactPoset& poset,
        int tab)
{
    num_calls += 1;

    if(partition_size < min_num_examples)
    {
//        assert(local_partition_index == 0 and partial_assignment_id == 0);
        partition_size = min_num_examples;
    }

//    assert(partition_size < all_meta_examples.ordering_of_partition_per_size.size());
    int partition_id = all_meta_examples.ordering_of_partition_per_size[partition_size][local_partition_index];
//    assert(partition_id < all_meta_examples.meta_examples.size());

    if(partial_assignment_id >= all_meta_examples.meta_examples[partition_id].size())
    {
        local_partition_index += 1;
        partial_assignment_id = 0;
        if(local_partition_index >= all_meta_examples.ordering_of_partition_per_size[partition_size].size())
        {
            partition_size += 1;
            local_partition_index = 0;
            if(partition_size > max_num_examples || partition_size >= all_meta_examples.ordering_of_partition_per_size.size())
            {
                num_dones += 1;
//                poset.record();
                poset.print();
                CompactPoset poset_copy = CompactPoset(&poset);

                if(num_dones%FREQUENCY_DONE_PRINT == 0)
                {
                    cout << "DONE #" << num_dones
                    << " num_nodes = " << poset.get_num_nodes()
                    << " num_remaining_meta_edges = " << poset.get_num_meta_edges()
                    << " num_inserted_meta_edges = " << poset.get_num_inserts()
                    << " num_decision_tree_nodes = " << get__global_num_decision_tree_nodes()
                    << " num_empty_slots = " << get__empty_slots_count()
                    << " time: " << (double)time(nullptr) - local_time << endl;

                }

                bool there_exist_redundant_meta_edge = poset_copy.soft_delete_redundant_edges();
                int diff_in_meta_edges = (poset.get_num_meta_edges() - poset_copy.get_num_meta_edges());
                if(there_exist_redundant_meta_edge)
                {
                    cout << "DONE #" << num_dones
                         << " num_nodes = " << poset_copy.get_num_nodes()
                         << " num_remaining_meta_edges = " << poset_copy.get_num_meta_edges()
                         << " num_inserted_meta_edges = " << poset_copy.get_num_inserts()
                         << " num_decision_tree_nodes = " << get__global_num_decision_tree_nodes()
                         << " num_empty_slots = " << get__empty_slots_count()
                         << " time: " << (double)time(nullptr) - local_time << endl;
                    cout << "redundant edges = " << diff_in_meta_edges << endl;
//                    assert(false);

                    if(poset_copy.get_num_meta_edges() <= 10)
                    {
                        poset_copy.print();
                    }
                }

                poset_copy.clear();

                return ;
            }
        }
        partition_id = all_meta_examples.ordering_of_partition_per_size[partition_size][local_partition_index];
        assert(partition_id < all_meta_examples.meta_examples.size());
    }

    for
    (
        int generalization_id = 0;
        generalization_id < all_meta_examples.meta_examples[partition_id][partial_assignment_id].size();
        generalization_id++
    )
    {
        if (tab < DEPTH_TREEE_PRINT) {
            for (int j = 0; j < tab; j++) {
                cout << "  ";
            }
            cout << "(" << partition_id << ", " << partial_assignment_id << ", " << generalization_id << "); " << endl;
        }

        if (poset.insert(all_meta_examples.meta_examples[partition_id][partial_assignment_id][generalization_id]))
        {
//            cout << "enter " << poset.get_num_nodes() << endl;
            enumerate_meta_training_sets(partition_size, local_partition_index, partial_assignment_id + 1, poset, tab + 1);
        }
        else
        {
//            cout << "exit" <<endl;
//            poset.print();
//            cout << endl;
        }
        poset.hard_pop();
    }
}


void enumerate_sets_of_meta_examples()
{
    int n = 1;

    all_meta_examples = AllMetaExamples(n, false);

    min_num_examples = 0;
    max_num_examples = 1;

    if(n == 2)
    {
        if(min_num_examples <= 1)
        {
            if(max_num_examples == 3)
            {
                DEPTH_TREEE_PRINT = 10;
                FREQUENCY_DONE_PRINT = 1;
            }
            else if(max_num_examples == 2)
            {
                DEPTH_TREEE_PRINT = 6;
                FREQUENCY_DONE_PRINT = 10000;
            }
            else if(max_num_examples == 1)
            {
                DEPTH_TREEE_PRINT = 0;
                FREQUENCY_DONE_PRINT = 1;
            }
            else
            {
                assert(false);
            }
        }
        else if(min_num_examples == 2)
        {
            DEPTH_TREEE_PRINT = 6;
            FREQUENCY_DONE_PRINT = 10000;
        }
        else
        {
            assert(false);
        }
    }
    else if(n == 1)
    {
        DEPTH_TREEE_PRINT = 0;
        FREQUENCY_DONE_PRINT = 100;
    }
    else if(n >= 3)
    {
        if(min_num_examples <= 1) {
            if (max_num_examples == 1) {
                DEPTH_TREEE_PRINT = 0;
                FREQUENCY_DONE_PRINT = 100;
            }
            else
            {
                assert(false);
            }
        }
        else
        {
            assert(false);
        }
    }
    else
    {
        assert(false);
    }


    current_file.open("n_" + std::to_string(n) + "__min_" + std::to_string(min_num_examples) + "__max_" +
                      std::to_string(max_num_examples) + ".out");

    current_file << std::to_string(n) <<" "<< std::to_string(min_num_examples) <<" "<< std::to_string(max_num_examples) << endl;

//    long long num_possible = count_possible_meta_example_sets(
//            0, 0, 0);

//    cout << "num possible: " << num_possible << endl;

    CompactPoset new_poset_with_hard_pop = CompactPoset(n);
    enumerate_meta_training_sets(
            0,
            0,
            0,
            new_poset_with_hard_pop,
            0);

    cout << "NUM CALLS = " << num_calls << endl;
//    cout << "NUM POSSIBLE = " << num_possible << endl;
    cout << "NUM DONES = " << num_dones << endl;
    cout << "NUM INVALIDS = " << num_early_invalids << endl;
    cout << "SUM SAVED = " << sum_saved << endl;

    sort(leafs_saved_per_invalid.begin(), leafs_saved_per_invalid.end());
    reverse(leafs_saved_per_invalid.begin(), leafs_saved_per_invalid.end());
    for (auto &local_int: leafs_saved_per_invalid) {
        cout << local_int << " ";
    }
    cout << endl;

    current_file.close();
}

int main() {

    bool do__enumerate_sets_of_meta_examples = false;
    if(do__enumerate_sets_of_meta_examples)
    {
        enumerate_sets_of_meta_examples();
    }
    else
    {
        int id = 0;
        vector<int> results;
        vector<int> total_without_trivial;
        vector<int> total_num_meta_examples;
        vector<int> schema_based_examples;
        for(int language_id = 0; language_id < 1+0*10;language_id++)
        {
            int num_inputs = 3;
            int max_partition_size = -1;
            int macro_partition = -1;
            LanguagesOverBooleanFunctions language = LanguagesOverBooleanFunctions(num_inputs, language_id);
            language.enumerate();

            int function_size = (1 << num_inputs);
            int num_functions = (1 << function_size);
            int num_partitions = num_functions-1;

            if(macro_partition != -1)
            {
                num_partitions = (1<<macro_partition);
            }

            vector<MetaExample> meta_examples;

            auto partitions_by_size = vector<vector<int> >(function_size, vector<int>());
            auto meta_examples_per_total_function = vector<vector<MetaExample> >(num_functions, vector<MetaExample>());

//            auto meta_examples_per_total_function = vector<DecisionTree>(num_functions, PartialFunction(num_inputs, 0, 0));


            for (int partition = 0; partition < num_partitions; partition++) {
                partitions_by_size[__builtin_popcount(partition)].push_back(partition);
            }
            if(max_partition_size == -1)
            {
                max_partition_size = partitions_by_size.size()-1;
            }


            cout << "enumerate meta examples for language_" << language_id << endl;
//            cout << "partial -> total" << endl;

            int local_total_num_meta_examples = 0;
            int local_total_without_trivial = 0;

            for(int partition_size = 0;partition_size<=max_partition_size;partition_size++) {
//                cout << "partition_size = " << partition_size << " time: " << (double)time(nullptr) - local_time << endl;
                for (int partition_idx = 0; partition_idx < partitions_by_size[partition_size].size(); partition_idx++) {
                    int partition = partitions_by_size[partition_size][partition_idx];
                    int num_partition_assignments = (1 << partition_size);
                    for (int partial_assignment = 0;
                         partial_assignment < num_partition_assignments; partial_assignment++) {
                        int total_function = 0;
                        int j = 0;
                        for (int i = 0; i < function_size; i++) {
                            if (get_bit(partition, i)) {
                                total_function += (get_bit(partial_assignment, j) << i);
                                j++;
                            }
                        }
                        MetaExample local_meta_example =
                                language.get_meta_example(PartialFunction(num_inputs, total_function, partition));

                        bool add =
                                    true;
//                                !meta_examples_per_total_function[local_meta_example.generalization.generalization].contains(
//                                local_meta_example.partial_function);
                        for(int i = 0; i<meta_examples_per_total_function[local_meta_example.generalization.total_function].size(); i++)
                        {
                            MetaExample smaller_meta_example = meta_examples_per_total_function[local_meta_example.generalization.total_function][i];

                            if(local_meta_example.partial_function.is_generalization_of(smaller_meta_example.partial_function))
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
                            local_meta_example.idx = meta_examples.size();
                            meta_examples.push_back(local_meta_example);
                            meta_examples_per_total_function[local_meta_example.generalization.total_function].push_back(local_meta_example);
//                            meta_examples_per_total_function[local_meta_example.generalization.generalization].apply_operation(
//                                    my_union, new DecisionTree(local_meta_example.partial_function));
                        }
                    }
                }
//                cout << "partition_size = " << partition_size << " num_of_meta_examples = " << meta_examples.size() << " time: " << (double)time(nullptr) - local_time << endl;
            }

            cout << "enumerated " << meta_examples.size() <<" meta examples for language_" << language_id << endl;

            total_num_meta_examples.push_back(local_total_num_meta_examples);
            total_without_trivial.push_back(local_total_without_trivial);


            CompactPoset poset = CompactPoset(num_inputs);

            bool insert_in_monolith = false;

            if(insert_in_monolith) {

                assert(poset.get_num_nodes() == 1);
                time_t prev_time = (double) time(nullptr);
                cout << "INSERT " << meta_examples.size() << " meta edges in CompactPoset" << endl;
                for (int i = 0; i < meta_examples.size(); i++) {
                    if ((double) time(nullptr) - prev_time >= 3) {
//                for (int i = meta_examples.size()-1; i >=0  ; i--) {
                        cout << "meta_example_id = " << i
                             << " num_nodes = " << poset.get_num_nodes()
                             << " num_remaining_meta_edges = " << poset.get_num_meta_edges()
                             << " num_inserted_meta_edges = " << poset.get_num_inserts()
                             << " num_decision_tree_nodes = " << get__global_num_decision_tree_nodes()
                             << " num_empty_slots = " << get__empty_slots_count()
                             << " time: " << (double) time(nullptr) - local_time << endl;
                        prev_time = (double) time(nullptr);
                    }
                    assert(poset.insert(meta_examples[i]));

//                poset.print();
//                poset.compare_nodes();
                }

//            poset.print();

                int prev_num_meta_edges = poset.get_num_meta_edges();

                poset.soft_delete_redundant_edges();

//            cout << "classification of meta examples for language_" << language_id << " :: " << endl;
//            cout << "partial -> total  classification" << endl;
//            cout << poset.meta_examples_to_string() << endl;

                results.push_back(poset.get_num_meta_edges());
            }

//            poset.print();
//
//            int diff_in_meta_edges = (prev_num_meta_edges - poset.get_num_meta_edges());
//
//                cout << "DONE "
//                     << " num_nodes = " << poset.get_num_nodes()
//                     << " num_remaining_meta_edges = " << poset.get_num_meta_edges()
//                     << " num_inserted_meta_edges = " << poset.get_num_inserts()
//                     << " num_decision_tree_nodes = " << get__global_num_decision_tree_nodes()
//                     << " num_empty_slots = " << get__empty_slots_count()
//                     << " time: " << (double)time(nullptr) - local_time << endl;
//                cout << "redundant edges = " << diff_in_meta_edges << endl;
//
////                    assert(false);
////            cout << "poset.compress() = " << poset.compress() <<endl;


//            for(int j = (int) meta_examples.size()-1; j >= 0; j--)
//            {
//                poset.hard_pop();
//            }

            bool run_reasoning_schema = false;

            if(run_reasoning_schema) {

                vector<pair<int, int> > masks;
                vector<pair<int, int> > partition_masks;

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

                ReasoningSchema reasoning_schema = ReasoningSchema(num_inputs, meta_examples, masks, partition_masks);

                schema_based_examples.push_back(reasoning_schema.get_num_necessary_meta_examples());

                reasoning_schema.test(meta_examples);

            }

            vector<int> unit_masks;


            vector<CompactPoset> compact_posets;

            MinimalFactoringSchema(num_inputs, meta_examples, "language(n="+std::to_string(num_inputs)+",id="+std::to_string(language_id)+")");



//            assert(false);

//            tests model stored in poset.
//            poset.calc_dominator_unions();
//            poset.calc_downstream_unions();

//            contains_counter = 0;
//
//            poset.num_visited_nodes_during_query = 0;
//
//            for(int j = 0;j<meta_examples.size();j++)
//            {
//                vector<PartialFunction> result = poset.query(meta_examples[j].partial_function);
//                cout << "meta_example:" << endl;
//                meta_examples[j].print();
//                cout << "result:" << endl;
//                for(int k = 0; k < result.size(); k++)
//                {
//                    cout << result[k].to_string() << endl;
//                }
//                assert(result.size() == 1);
//                assert(result[0].total_function == meta_examples[i].generalization.total_function);
//            }
//
//            cout << "poset.num_visited_nodes_during_query = " << poset.num_visited_nodes_during_query <<endl;
//
//            cout << "is_generalization_counter = " << language.is_generalization_counter << endl;
//            cout << "contains_counter = " << contains_counter << endl;


            poset.clear();
        }

        for(int i = 0;i<results.size();i++)
        {
            cout << "language_" << i << " "<< total_num_meta_examples[i] << " & "<< total_without_trivial[i] << " & " << results[i] << " & " << schema_based_examples[i] << endl;
        }
    }


    cout << "TIME: " << (double) time(nullptr) - local_time << endl;

//
//    CompactPoset compact_poset = CompactPoset(2);
//    cout << "is_valid_node: " << compact_poset.insert(MetaExample(PartialFunction(2, 0, 0))) <<endl;
//    cout << "is_valid: " << compact_poset.is_valid() <<endl;
//
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    cout << "is_valid_node: " << compact_poset.insert(MetaExample(PartialFunction(2, 5, 1))) << endl;
//    cout << "is_valid: " << compact_poset.is_valid() <<endl;
//
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    cout << "is_valid_node: " << compact_poset.insert(MetaExample(PartialFunction(2, 7, 1))) << endl;
//    cout << "is_valid: " << compact_poset.is_valid() <<endl;
//
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    compact_poset.hard_pop();
//    cout << "is_valid_node: " << compact_poset.is_valid() << endl;
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    compact_poset.hard_pop();
//    cout << "is_valid_node: " << compact_poset.is_valid() << endl;
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    compact_poset.hard_pop();
//    cout << "is_valid_node: " << compact_poset.is_valid() << endl;
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;


//    PartialFunction all = PartialFunction(2, 0, 0);
//    DecisionTree dt_all = DecisionTree(all);
//    cout << "dt_all::" <<endl;
//    cout << dt_all.to_string(2) << endl << endl;
//
//    PartialFunction only_0 = PartialFunction(2, 0, (1<<(1<<2))-1);
//    DecisionTree dt_only_0 = DecisionTree(only_0);
//    cout << "only_0::" <<endl;
//    cout << dt_only_0.to_string(2) << endl << endl;
//
//    cout << "dt_all difference only_0" <<endl;
//    dt_all.apply_operation(difference, &dt_only_0);
//    cout << dt_all.to_string(2) << endl << endl;
//
//
//    PartialFunction all_init_1 = PartialFunction(2, 5, 1);
//    DecisionTree dt_all_init_1 = DecisionTree(all_init_1);
//    cout << "dt_all_init_1::" <<endl;
//    cout << dt_all_init_1.to_string(2) << endl << endl;
//
//    PartialFunction only_5 = PartialFunction(2, 5, (1<<(1<<2))-1);
//    DecisionTree dt_only_5 = DecisionTree(only_5);
//    cout << "dt_only_5::" <<endl;
//    cout << dt_only_5.to_string(2) << endl << endl;
//
//    cout << "dt_all_init_1 difference dt_only_5" <<endl;
//    dt_all_init_1.apply_operation(difference, &dt_only_5);
//    cout << dt_all_init_1.to_string(2) << endl << endl;
//
//
//    cout << "(dt_all difference only_0) difference (dt_all_init_1 difference dt_only_5)" <<endl;
//    dt_all.apply_operation(difference, &dt_all_init_1);
//    cout << dt_all.to_string(2) << endl << endl;


    return 0;
}


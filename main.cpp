#include <iostream>
#include "CompactPoset.h"
#include <fstream>



//class MetaExample {
//public:
//    int num_inputs;
//    int total_function_size;
//    int total_function_output;
//    int partition;
//
//    int partial_function_size;
//    int generalization_size;
//    int partial_function_outputs = 0;
//    int generalization_outputs = 0;
//
//    vector<TotalFunction> dominated_total_functions;
//
//    MetaExample(int _num_inputs, int _total_function_output, int _partition){
//        num_inputs = _num_inputs;
//        total_function_size = (1<<num_inputs);
//        total_function_output = _total_function_output;
//        partition = _partition;
//        partial_function_size = __builtin_popcount(partition);
//        calc__partial_function_outputs();
//    }
//
//    void prep_for_use()
//    {
//        calc__generalization_outputs();
//        generalization_size = total_function_size - partial_function_size;
//        int dominator_output_bits = generalization_outputs;
//        for (int dominated_output_bits = 0;
//             dominated_output_bits < (1 << generalization_size); dominated_output_bits++) {
//            if (dominated_output_bits != dominator_output_bits) {
//                int dominated_total_function = 0;
//                for (int j = 0, k = 0; j < total_function_size; j++) {
//                    if (!test_bit(partition, j)) {
//                        dominated_total_function += (1 << j) * test_bit(dominated_output_bits, k);
//                        k++;
//                    } else {
//                        dominated_total_function += (1 << j) * test_bit(total_function_output, j);
//                    }
//                }
//                assert(dominated_total_function != total_function_output);
//                dominated_total_functions.emplace_back(
//                        TotalFunction(num_inputs, total_function_size, dominated_total_function));
//            }
//        }
//    }
//
//    void calc__partial_function_outputs()
//    {
//        for(int i = 0, j = 0;i<total_function_size; i++) {
//            if (test_bit(partition, i)) {
//                partial_function_outputs += (1 << j) * test_bit(total_function_output, i);
//                j++;
//            }
//        }
//    }
//
//    void calc__generalization_outputs()
//    {
//        for(int i = 0, j = 0;i<total_function_size; i++) {
//            if (!test_bit(partition, i)) {
//                generalization_outputs += (1 << j) * test_bit(total_function_output, i);
//                j++;
//            }
//        }
//    }
//
//    string linear_string(int tab)
//    {
//        string ret;
//        for(int i = 0;i<tab;i++)
//        {
//            ret+="\t";
//        }
//        ret += "{";
//        ret += "(";
//        bool is_first = true;
//        for(int i = 0;i<total_function_size;i++)
//        {
//            if(test_bit(partition, i))
//            {
//                if(!is_first)
//                {
//                    ret += " && ";
//                }
//                else
//                {
//                    is_first = false;
//                }
//                ret += bitvector_to_str(i, num_inputs);
//                ret += "->";
//                ret += bitvector_to_str(test_bit(total_function_output, i), 1);
//
//            }
//        }
//        ret += ") ";
//        ret += "-->";
//        ret += " (";
//        is_first = true;
//        for(int i = 0;i<total_function_size;i++)
//        {
//            if(!test_bit(partition, i))
//            {
//                if(!is_first)
//                {
//                    ret += " && ";
//                }
//                else
//                {
//                    is_first = false;
//                }
//                ret += bitvector_to_str(i, num_inputs);
//                ret += "->";
//                ret += bitvector_to_str(test_bit(total_function_output, i), 1);
//            }
//        }
//        ret += ")";
//        ret += "}";
//        return ret;
//    }
//
//    string linear_string()
//    {
//        return linear_string(0);
//    }
//};
//

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
                                << meta_examples[partition][j][k].linear_string() << endl;
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
//                poset.to_string();
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

                CompactPoset poset_copy = CompactPoset(&poset);

                if(poset_copy.there_exist_redundant_meta_edge())
                {

                    cout << "DONE #" << num_dones
                         << " num_nodes = " << poset_copy.get_num_nodes()
                         << " num_remaining_meta_edges = " << poset_copy.get_num_meta_edges()
                         << " num_inserted_meta_edges = " << poset_copy.get_num_inserts()
                         << " num_decision_tree_nodes = " << get__global_num_decision_tree_nodes()
                         << " num_empty_slots = " << get__empty_slots_count()
                         << " time: " << (double)time(nullptr) - local_time << endl;
                    cout << "redundant edges = " << (poset.get_num_meta_edges() - poset_copy.get_num_meta_edges()) << endl;
//                    assert(false);
                }

                poset_copy.add_edges_back();
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

        if (poset.append(all_meta_examples.meta_examples[partition_id][partial_assignment_id][generalization_id]))
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
        poset.pop();
    }
}


void enumerate_sets_of_meta_examples()
{
    int n = 2;

    all_meta_examples = AllMetaExamples(n, false);

    min_num_examples = 2;
    max_num_examples = 2;

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
                FREQUENCY_DONE_PRINT = 1;
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
            FREQUENCY_DONE_PRINT = 1;
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

    enumerate_sets_of_meta_examples();

    cout << "TIME: " << (double) time(nullptr) - local_time << endl;

//
//    CompactPoset compact_poset = CompactPoset(2);
//    cout << "is_valid_node: " << compact_poset.append(MetaExample(PartialFunction(2, 0, 0))) <<endl;
//    cout << "is_valid: " << compact_poset.is_valid() <<endl;
//
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    cout << "is_valid_node: " << compact_poset.append(MetaExample(PartialFunction(2, 5, 1))) << endl;
//    cout << "is_valid: " << compact_poset.is_valid() <<endl;
//
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    cout << "is_valid_node: " << compact_poset.append(MetaExample(PartialFunction(2, 7, 1))) << endl;
//    cout << "is_valid: " << compact_poset.is_valid() <<endl;
//
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    compact_poset.pop();
//    cout << "is_valid_node: " << compact_poset.is_valid() << endl;
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    compact_poset.pop();
//    cout << "is_valid_node: " << compact_poset.is_valid() << endl;
//    cout << "compact_poset:: " <<endl;
//    cout << compact_poset.to_string() << endl;
//
//    compact_poset.pop();
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




//
// Created by Kliment Serafimov on 1/6/20.
//

#include "enumerate_meta_training_sets.h"

void enumerate_meta_training_sets()
{
    int n = 2;

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

    CompactPoset new_poset_with_hard_pop = CompactPoset((1<<n));
    recursively_enumerate_meta_training_sets(
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

void recursively_enumerate_meta_training_sets(
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
            recursively_enumerate_meta_training_sets(
                    partition_size, local_partition_index, partial_assignment_id + 1,poset, tab + 1);
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


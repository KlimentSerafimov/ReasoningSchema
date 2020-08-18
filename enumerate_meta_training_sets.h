//
// Created by Kliment Serafimov on 1/6/20.
//

#ifndef COMPACTPOSET_ENUMERATE_META_TRAINING_SETS_H
#define COMPACTPOSET_ENUMERATE_META_TRAINING_SETS_H

#include <iostream>
#include <ctime>
#include <fstream>
#include "CompactPoset.h"

using namespace std;

static int min_num_examples;
static int max_num_examples;

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
                MetaExample local_meta_example = MetaExample((1<<num_inputs), output_bits, partition);
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
//                    MetaExample exstended_meta_example = MetaExample(function_size, output_bits, over_partition);
//
//                    skip_meta_examples[partition][compact_partial_outputs][local_generalization_id].my_push_back(
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
                                //                        << bitvector_to_str(generalization_output, (1<<function_size)-size_of_partial_function) << endl;
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

static AllMetaExamples all_meta_examples = AllMetaExamples();

static ofstream current_file;

static int num_dones = 0;
static int num_early_invalids = 0;
static int sum_saved = 0;
static vector<int> leafs_saved_per_invalid;
static int num_calls = 0;

static time_t local_time = time(nullptr);

static int DEPTH_TREEE_PRINT;
static int FREQUENCY_DONE_PRINT;

void recursively_enumerate_meta_training_sets(
        int partition_size,
        int local_partition_index,
        int partial_assignment_id,
        CompactPoset& poset,
        int tab);

void enumerate_meta_training_sets();

#endif //COMPACTPOSET_ENUMERATE_META_TRAINING_SETS_H

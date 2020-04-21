//
// Created by Kliment Serafimov on 2019-12-23.
//

#include "Module.h"
#include "ReasoningSchemaOptimizer.h"
#include "util.h"
#include <iostream>

string Module::subdomain_mask_to_string(Bitvector local_subdomain_mask)
{
    return parent_minimal_factoring_schema->root_pointer->bitvector_to_string__one_line(local_subdomain_mask);
}

string Module::meta_example_to_string(MetaExample meta_example)
{
    return parent_minimal_factoring_schema->root_pointer->meta_example_to_string__one_line(meta_example);
}

string Module::print_module_sketch(time_t delta_time)
{

    string one_line = subdomain_mask_to_string(subdomain_mask);

    string ret =
            one_line + " | " +
            bitvector_to_str(subdomain_mask, function_size) + " " +
            std::to_string(necessary_meta_example_ids.size()) + " " +
            std::to_string(intermediate_num_missing_bits) + " time " + std::to_string(delta_time) + "\n";

    assert(num_missing_bits_per_repeat.size() == repeats_module_ids.size());
    assert(num_missing_bits_per_repeat.size() == repeats_module_pointers.size());

    for(int i = 0;i<repeats_module_ids.size();i++)
    {
        string one_line_1 = subdomain_mask_to_string(repeats_module_pointers[i]->subdomain_mask);
        ret +=
                one_line_1 + " | "+
                bitvector_to_str(repeats_module_pointers[i]->subdomain_mask, repeats_module_pointers[i]->function_size) + " " +
                std::to_string(necessary_meta_example_ids.size()) + " " +
                std::to_string(num_missing_bits_per_repeat[i]) + " " +
                "mask " + std::to_string(repeats_module_ids[i]) + " " +
                "( " +
                one_line_1 + " " +
//                bitvector_to_str(repeats_module_pointers[i]->subdomain_mask, repeats_module_pointers[i]->function_size) + " " +
                std::to_string(repeats_module_pointers[i]->necessary_meta_example_ids.size()) + " " +
                std::to_string(repeats_module_pointers[i]->num_missing_bits) + " " +
                ") time " + std::to_string(delta_time) + "\n";
    }
    return ret;
}

string Module::covered_to_string(vector<MetaExample> init_meta_examples)
{
    string ret;
    vector<Module*> parent_modules;

    Module* at_module = this;
    while(at_module != nullptr)
    {
        parent_modules.push_back(at_module);
        at_module = at_module->parent_module;
    }

    reverse(parent_modules.begin(), parent_modules.end());

    assert(covering.size() == parent_modules.size());

    for(int i = (int) 0;i<covering.size();i++)
    {
        ret += "subdomain_mask " + subdomain_mask_to_string(parent_modules[i]->subdomain_mask) + " id " + std::to_string(i) + "\n";
        for(int j = 0;j<covering[i].size(); j++)
        {
            assert(covering[i][j].size() >= 1);
            for(int k = 0; k < covering[i][j].size(); k++) {
                PartialFunction partial_function =
                    init_meta_examples[covering[i][j][k]].partial_function;
                PartialFunction intermediate_partial_function =
                    parent_minimal_factoring_schema->root_pointer->query(partial_function, parent_modules[i]);

//                ret += "pre_query " + partial_function.to_string()+"\t";
//                ret += "after_query " + intermediate_partial_function.to_string()+"\t";
                MetaExample intermediate_meta_example = MetaExample(
                        intermediate_partial_function.function_size,
                        init_meta_examples[covering[i][j][k]].generalization.total_function,
                        intermediate_partial_function.partition);
                if (k >= 1) {
                    ret += "\t\tequiv ";
                }
                else
                {
                    ret += "\t";
                }
                ret +=
//                        "subdomain_mask " + bitvector_to_str(parent_modules[i]->subdomain_mask, function_size) + "\t" +
                        "masked " + meta_example_to_string(intermediate_meta_example.get_application_of_subdomain(
                                parent_modules[i]->subdomain_mask)) + "\t" +
//                intermediate_meta_example.get_application_of_subdomain(
//                        parent_modules[i]->subdomain_mask).to_string() + "\t" +
                "intermediate " + meta_example_to_string(intermediate_meta_example) + "\t" +
                //intermediate_meta_example.to_string() + "\t" +
                        "original " +  meta_example_to_string(init_meta_examples[covering[i][j][k]]) + "\t";
//                        init_meta_examples[covering[i][j][k]].to_string() + "\t";

            }
            ret+="\n";
        }
    }
    return ret;
}
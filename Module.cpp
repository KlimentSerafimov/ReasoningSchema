//
// Created by Kliment Serafimov on 2019-12-23.
//

#include "Module.h"
#include <iostream>

string Module::print_module_sketch(time_t delta_time)
{
    string ret =
            bitvector_to_str(subdomain_mask, function_size) + " " +
            std::to_string(necessary_meta_example_ids.size()) + " " +
            std::to_string(intermediate_num_missing_bits) + " time " + std::to_string(delta_time) + "\n";

    for(int i = 0;i<repeats.size();i++)
    {
        ret +=
                bitvector_to_str(repeats[i].second->subdomain_mask, repeats[i].second->function_size) + " " +
                std::to_string(necessary_meta_example_ids.size()) + " " +
                std::to_string(num_missing_bits) + " " +
                "mask " + std::to_string(repeats[i].first) + " " +
                "( " +
                bitvector_to_str(repeats[i].second->subdomain_mask, repeats[i].second->function_size) + " " +
                std::to_string(repeats[i].second->necessary_meta_example_ids.size()) + " " +
                std::to_string(repeats[i].second->num_missing_bits) + " " +
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

    assert(covered.size() == parent_modules.size());

    for(int i = (int) covered.size()-1;i>=0;i--)
    {
        ret += bitvector_to_str(parent_modules[i]->subdomain_mask, parent_modules[i]->function_size) + " " + "\n";
        for(int j = 0;j<covered[i].size(); j++)
        {
            assert(covered[i][j].size() >= 1);
            for(int k = 0; k < covered[i][j].size(); k++) {
                ret += "\t" + init_meta_examples[covered[i][j][k]].to_string() + " ";
                if (k == 0 && covered[i][j].size() >= 2) {
                    ret += "alternatives ";
                }
            }
            ret+="\n";
        }
    }
    return ret;
}